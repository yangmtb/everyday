package main

import (
	"common"
	"net"
	"time"
)

const (
	constheart = byte(0)
	constinfo  = byte('i')
)

type Clt struct {
	cid     int
	conn    net.Conn
	version byte
	source  string
	info    common.CltInfo
}

func NewClt(conn net.Conn, version byte, source string) (clt *Clt) {
	clt = new(Clt)
	clt.conn = conn
	clt.version = version
	clt.source = source
	return
}

func (c *Clt) Handle() {
	clientList.AddClient(c)
	defer client.DelClient(c)
	flag := make([]byte, 1)
	n := 0
	var err error
	for {
		c.conn.SetDeadline(time.Now().Add(180 * time.Second))
		if n, err = c.conn.Read(flag); nil != err || len(flag) != n {
			return
		}
		switch flag[0] {
		case constheart:
			if n, err = c.conn.Write(flag); nil != err || n != len(flag) {
				return
			}
			continue
		case constinfo:
			if err = c.recvInfo(); nil != err {
				return
			}
		default:
			fmt.Println("other", flag)
			return
		}

	}
}

func (c *Clt) GetInfo() {
	c.conn.Write(constinfo)
}

func (c *Clt) recvInfo() (err error) {
	infob := make([]byte, length)
	if err = readXBytes(c.conn, infob); nil != err {
		return
	}
	if err = json.Unmarshal(da, &c.info); nil != err {
		return
	}
	return nil
}
