package main

import (
	"fmt"
	"net"
	//"strconv"
	"sync"
	"time"

	"github.com/gorilla/websocket"
)

func main() {
	var wg sync.WaitGroup
	err := 0
	cnt := 0
	var mtx sync.Mutex
	for i := 0; i < 200; i++ {
		wg.Add(1)
		go func(i int) {
			defer wg.Done()
			err0, cnt0 := sh(i)
			mtx.Lock()
			err += err0
			cnt += cnt0
			mtx.Unlock()
			//fmt.Println("dial done", i)
		}(i)
		time.Sleep(1 * time.Millisecond)
	}
	wg.Wait()
	fmt.Println(cnt, "main done", err)
	return
}

func sh(id int) (err, cnt int) {
	var wg sync.WaitGroup
	var mtx sync.Mutex
	for i := 0; i < 200; i++ {
		wg.Add(1)
		go func(i int) {
			defer wg.Done()
			ret := tasktcp(i)
			mtx.Lock()
			if -1 == ret {
				err++
			}
			cnt++
			mtx.Unlock()
			//fmt.Println("dial done", i)
		}(i)
		time.Sleep(1 * time.Millisecond)
	}
	wg.Wait()
	if (err > 0) {
		fmt.Println(id, cnt, "sh done", err)
	}
	return
}

func taskws() {
	c, _, err := websocket.DefaultDialer.Dial("ws://127.0.0.1:9999", nil)
	if nil != err {
		fmt.Println("dial err:", err)
		return
	}
	defer c.Close()
	done1 := make(chan struct{})
	go func() {
		defer close(done1)
		for {
			_, msg, err := c.ReadMessage()
			if nil != err {
				fmt.Println("read:", err)
				return
			}
			fmt.Println("recv:", msg)
		}
	}()
	ticker := time.NewTicker(time.Second)
	defer ticker.Stop()
	for {
		select {
		case <-done1:
			return
		case t := <-ticker.C:
			err := c.WriteMessage(websocket.TextMessage, []byte("xx"))
			if nil != err {
				fmt.Println("write:", err, t)
				return
			}
		}
	}
	return
}

func tasktcp(i int) int {
	conn, err := net.Dial("tcp", "localhost:9999")
	if nil != err {
		//fmt.Println(i, "dial err:", err)
		return -1
	}
	//time.Sleep(time.Duration((i+1)%10+1) * time.Millisecond)
	//fmt.Println("dial", i)
	//time.Sleep(time.Duration(1 * time.Millisecond))
	conn.Close()
	return 0
	done := make(chan string)
	go handleWrite(conn, done)
	go handleRead(conn, done)
	<-done
	<-done
	//fmt.Println(<-done)
	//fmt.Println(<-done)
	return 0
}

func handleWrite(conn net.Conn, done chan string) {
	for i := 10; i > 0; i-- {
		//_, e := conn.Write([]byte("hello " + strconv.Itoa(i) + ""))
		_, e := conn.Write([]byte("hello"))
		if nil != e {
			fmt.Println("send error:", e)
			break
		}
		//time.Sleep(1*time.Millisecond + 1)
	}
	done <- "sent"
}

func handleRead(conn net.Conn, done chan string) {
	time.Sleep(1 * time.Second)
	buf := make([]byte, 1024)
	reqLen, err := conn.Read(buf)
	if nil != err {
		fmt.Println("read error:", err, reqLen)
		return
	}
	if (reqLen != 50) {
		fmt.Println(reqLen, "recv:", string(buf[:reqLen-1]))
	}
	done <- "read"
}
