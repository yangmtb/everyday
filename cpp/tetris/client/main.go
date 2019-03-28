package main

import (
	"fmt"
	"net"
	"strconv"
	"time"
	"sync"

	"github.com/gorilla/websocket"
)

func main() {
	var wg sync.WaitGroup
	for i:=0; i<10; i++ {
		wg.Add(1)
		go func(i int) {
			defer wg.Done()
			tasktcp(i)
			fmt.Println("dial done", i)
		}(i)
	}
	wg.Wait()
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

func tasktcp(i int) {
	conn, err := net.Dial("tcp", "localhost:9999")
	if nil != err {
		fmt.Println("dial err:", err)
		return
	}
	defer conn.Close()
	time.Sleep(time.Duration((i+1)%10)*time.Second)
	fmt.Println("dial", i)
	//time.Sleep(time.Duration(10*time.Second))
	//return
	done := make(chan string)
	go handleWrite(conn, done)
	go handleRead(conn, done)
	<-done
	<-done
	//fmt.Println(<-done)
	//fmt.Println(<-done)
}

func handleWrite(conn net.Conn, done chan string) {
	for i := 10; i > 0; i-- {
		_, e := conn.Write([]byte("hello " + strconv.Itoa(i) + ""))
		if nil != e {
			fmt.Println("send error:", e)
			break
		}
		time.Sleep(100*time.Millisecond)
	}
	done <- "sent"
}

func handleRead(conn net.Conn, done chan string) {
	buf := make([]byte, 1024)
	reqLen, err := conn.Read(buf)
	if nil != err {
		fmt.Println("read error:", err)
		return
	}
	fmt.Println(reqLen, "recv:", string(buf[:reqLen-1]))
	done <- "read"
}
