#include <iostream>
#include <cstdio>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <SDL2/SDL.h>
#ifdef __cplusplus
}
#endif // __cplusplus

// refresh event
#define SFM_REFRESH_EVENT (SDL_USEREVENT+1)
#define SFM_BREAK_EVENT (SDL_USEREVENT+2)

int thread_exit = 0;

int sfp_refresh_thread(void *opaque)
{
    thread_exit = 0;
    while (!thread_exit) {
        SDL_Event event;
        event.type = SFM_REFRESH_EVENT;
        SDL_PushEvent(&event);
        SDL_Delay(40);
    }
    thread_exit = 0;
    // break
    SDL_Event event;
    event.type = SFM_BREAK_EVENT;
    SDL_PushEvent(&event);

    return 0;
}

using namespace std;

int main(int argc, char *argv[])
{
    cout << "Hello world!" << endl;
    int idx = -1;
    AVFormatContext *fmtctx;
    AVCodec *codec;
    AVCodecContext *codecctx;
    AVCodecParameters *codecpar;

    avformat_network_init();
    avdevice_register_all();

    fmtctx = avformat_alloc_context();

    // linux
    AVDictionary *options = nullptr;
    AVInputFormat *infmt = av_find_input_format("x11grab");
    if (avformat_open_input(&fmtctx, ":0.0+100, 200", infmt, &options) != 0) {
        cerr << "can't open input stream" << endl;
        return -1;
    }
    if (avformat_find_stream_info(fmtctx, nullptr) < 0) {
        cerr << "can't find stream information." << endl;
        return -1;
    }
    for (unsigned int i = 0; i < fmtctx->nb_streams; ++i) {
        if (AVMEDIA_TYPE_VIDEO == fmtctx->streams[i]->codecpar->codec_type) {
            idx = i;
            break;
        }
    }
    cout << "idx:" << idx << endl;
    if (-1 == idx) {
        cerr << "don't find a video stream." << endl;
        return -1;
    }
    codecpar = fmtctx->streams[idx]->codecpar;
    codecctx = fmtctx->streams[idx]->codec;
    codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec) {
        cerr << "can't find codec." << endl;
        return -1;
    }
    //codecctx = avcodec_alloc_context3(codec);
    if (avcodec_open2(codecctx, codec, nullptr) < 0) {
        cerr << "can't open codec." << endl;
        return -1;
    }
    AVFrame *frame, *frameYUV;
    frame = av_frame_alloc();
    frameYUV = av_frame_alloc();
    unsigned char *buf = (unsigned char *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, codecpar->width, codecpar->height));
    avpicture_fill((AVPicture *)frameYUV, buf, AV_PIX_FMT_YUV420P, codecpar->width, codecpar->height);

    // sdl
    int ret = 0;
    if (0 != (ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))) {
        cerr << ret << " sdl initialize boom:" << SDL_GetError() << endl;
        return -1;
    }
    cout << "init done " << ret << endl;
    int w = 640, h = 480;
    SDL_Window *screen = SDL_CreateWindow("sdl2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if (!screen) {
        cerr << "create window:" << SDL_GetError() << endl;
        return -1;
    }
    cout << "create done" << endl;
    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, 0);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, w, h);
    SDL_Thread *tid = SDL_CreateThread(sfp_refresh_thread, nullptr, nullptr);

    SwsContext *convert = sws_getContext(codecpar->width, codecpar->height, (AVPixelFormat)codecpar->format, w, h, AV_PIX_FMT_YUV420P, SWS_BICUBIC, nullptr, nullptr, nullptr);

    SDL_Event event;
    AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));

    while (1) {
        SDL_WaitEvent(&event);
        if (SFM_REFRESH_EVENT == event.type) {
            if (av_read_frame(fmtctx, packet) >= 0) {
                ret = avcodec_send_packet(codecctx, packet);
                if (ret < 0) {
                    cerr << "error "<< ret << endl;
                    break;
                }
                while (ret >= 0) {
                    ret = avcodec_receive_frame(codecctx, frame);
                    if (AVERROR(EAGAIN) == ret || AVERROR_EOF == ret) {
                        break;
                    } else if (ret < 0) {
                        cerr << "ret < 0" << endl;
                        break;
                    }
                    sws_scale(convert, frame->data, frame->linesize, 0, codecpar->height, frameYUV->data, frameYUV->linesize);
                    SDL_UpdateTexture(texture, nullptr, frameYUV->data[0], frameYUV->linesize[0]);
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
                    SDL_RenderPresent(renderer);
                }
            } else {
                thread_exit = 1;
            }
        } else if (SDL_QUIT == event.type) {
            thread_exit = 1;
        } else if (SFM_BREAK_EVENT == event.type) {
            break;
        }
    }

    sws_freeContext(convert);

    SDL_Quit();

    avcodec_close(codecctx);
    avformat_close_input(&fmtctx);

    cout << "all done" << endl;

    return 0;
}
