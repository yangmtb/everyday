package main

import (
	"common"
	"encoding/json"
	"errors"
	"fmt"
	"log"
	"net"
	"time"
)

func main() {
	t := common.CltInfo{}
	t.ID = 1
	fmt.Println(t)
	svr, err := net.Listen("tcp", "0.0.0.0:5555")
	if nil != err {
		log.Fatal(err)
	}
	defer svr.Close()
	for {
		conn, err := svr.Accept()
		if nil != err {
			continue
		}
		go handleConnection(conn)
	}
}

func handleConnection(conn net.Conn) {
	defer conn.Close()
	conn.SetDeadline(time.Now().Add(24 * time.Hour))
	for {
		ty, err := readXBytes(conn, 1)
		if nil != err {
			fmt.Println("err", err)
			break
		}
		switch ty[0] {
		case 'x':
			err = recvExe(conn)
			fmt.Println("exe done")
		case 'h':
			err = recvHeart(conn)
			if nil != err {
				fmt.Println("heart err")
				break
			}
			fmt.Println("heart done")
		case 'd':
			fmt.Println("down")
			break
		}
	}
	fmt.Println("conn will close")
}

func recvExe(conn net.Conn) (err error) {
	len, err := readXBytes(conn, 1)
	if nil != err {
		return err
	}
	//fmt.Println("len:", len)
	da, err := readXBytes(conn, int(len[0]))
	if nil != err {
		return
	}
	fmt.Println(string(da))
	return
}

func recvHeart(conn net.Conn) (err error) {
	len, err := readXBytes(conn, 1)
	if nil != err {
		return err
	}
	//fmt.Println("len:", len)
	da, err := readXBytes(conn, int(len[0]))
	if nil != err {
		return
	}
	var info common.CltInfo
	err = json.Unmarshal(da, &info)
	fmt.Println(info)
	return
}

func readXBytes(conn net.Conn, amount int) ([]byte, error) {
	buf := make([]byte, amount)
	tl := 0
	for tl < amount {
		rd, err := conn.Read(buf[tl:])
		if nil != err || rd <= 0 {
			return nil, err
			return nil, errors.New("failed to read")
		}
		tl += rd
	}
	return buf, nil
}
