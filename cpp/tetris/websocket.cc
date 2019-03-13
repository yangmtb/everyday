#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <iostream>
#include <string>
#include "shape.hpp"

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using std::string;
using std::stringstream;
using std::cout;
using std::endl;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

Shape *layout = new Shape();
Shape *a = nullptr;//new Shape(layout);

bool validate(server *s, websocketpp::connection_hdl hdl) {
  // do something
  return true;
}

void on_http(server *s, websocketpp::connection_hdl hdl) {
  server::connection_ptr con = s->get_con_from_hdl(hdl);
  string res = con->get_request_body();
  stringstream ss;
  ss << "got HTTP request with " << res.size() << " bytes of body data.";
  con->set_body(ss.str());
  con->set_status(websocketpp::http::status_code::ok);
}

void on_fail(server *s, websocketpp::connection_hdl hdl) {
  server::connection_ptr con = s->get_con_from_hdl(hdl);
  cout << "fail handler: " << con->get_ec() << " " << con->get_ec().message() << endl;
}

void on_close(websocketpp::connection_hdl hdl) {
  cout << "close handler: " << hdl.lock().get() << endl;
}

// define a callback to handle incoming message
void on_message(server *s, websocketpp::connection_hdl hdl, message_ptr msg) {
  cout << "on_message called with hdl: " << hdl.lock().get() << " and message: " << msg->get_payload() << " opcode:" << msg->get_opcode() << endl;
  // check for a special command to instruct the server to stop listening so it can be cleanly exited.
  if ("stop-listening" == msg->get_payload()) {
    s->stop_listening();
    return;
  } else if ("begin" == msg->get_payload()) {
    cout << "game begin" << endl;
    a = new Shape(layout);
    if (nullptr == a) {
      cout << "a is nullptr begin" << endl;
      s->stop_listening();
      return;
    }
  } else if ("left" == msg->get_payload()) {
    if (nullptr == a) {
      cout << "a is nullptr left" << endl;
      s->stop_listening();
      return;
    }
    a->Left();
  } else if ("right" == msg->get_payload()) {
    a->Right();
  } else if ("rotate" == msg->get_payload()) {
    a->Rotate();
  } else if ("down" == msg->get_payload()) {
    if (nullptr == a) {
      cout << "a is nullptr down" << endl;
      s->stop_listening();
      return;
    }
    if (!a->Down()) {
      cout << "time to new a shape" << endl;
      cout << "eliminate:" << layout->Eliminate() << endl;
      a = new Shape(layout);
      if (!a->IsValid()) {
        cout << "game over" << endl;
        s->stop_listening();
        return;
      }
    }
  } else {
    cout << "boom :" << msg->get_payload() << endl;
    s->stop_listening();
    return;
  }
  //a->Show();
  string content = a->GetString();
  try {
    s->send(hdl, content, msg->get_opcode());
    //s->send(hdl, msg->get_payload(), msg->get_opcode());
  } catch (websocketpp::exception const & e) {
    cout << "echo failed because: (" << e.what() << ")" << endl;
  }
}

int main(const int argc, const char *argv[]) {
  server echo_server;
  try {
    // set logging settings
    echo_server.set_access_channels(websocketpp::log::alevel::all);
    echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);
    // initialize asio
    echo_server.init_asio();
    echo_server.set_reuse_addr(true);
    // register our message handler
    echo_server.set_message_handler(bind(&on_message, &echo_server, ::_1, ::_2));
    echo_server.set_http_handler(bind(&on_http, &echo_server, ::_1));
    echo_server.set_fail_handler(bind(&on_fail, &echo_server, ::_1));
    echo_server.set_close_handler(&on_close);
    echo_server.set_validate_handler(bind(&validate, &echo_server, ::_1));
    // listen on port 9999
    echo_server.listen(9999);
    // start the server accept loop
    echo_server.start_accept();
    // start the asio io_service run loop
    echo_server.run();
  } catch (websocketpp::exception const & e) {
    cout << "exception:" << e.what() << endl;
  } catch (...) {
    cout << "other exception" << endl;
  }
}
