package main

import (
	"common"
	"encoding/json"
	"fmt"
	"log"
	"net"
	"time"
)

var info common.CltInfo

func sendHeart(conn net.Conn) {
	i := 0
	for {
		i++
		buf, err := json.Marshal(info)
		if nil != err {
			fmt.Println("json err", err)
			return
		}
		tmp := make([]byte, len(buf)+2)
		tmp = append(tmp, byte('h'))
		tmp = append(tmp, byte(len(buf)))
		tmp = append(tmp, buf...)
		n, err := conn.Write(tmp)
		fmt.Println(i, n, "heart done", err)
		time.Sleep(3 * time.Second)
	}
}

func main() {
	conn, err := net.Dial("tcp", "127.0.0.1:5555")
	if nil != err {
		log.Fatal(err)
	}
	defer conn.Close()
	info.ID = 1
	info.CPU = 10
	info.Memory = 20
	info.Name = "yes"
	info.TaskCount = 2
	go sendHeart(conn)

	i := 0
	for i < 55 {
		i++

		go func(i int) {
			tp := make([]byte, 4)
			tp = append(tp, byte('x'))
			tp = append(tp, byte(4))
			tp = append(tp, []byte("aabb")...)
			n, err := conn.Write(tp)
			fmt.Println(i, n, "x done", err)
		}(i)

		time.Sleep(2 * time.Second)
	}
}
