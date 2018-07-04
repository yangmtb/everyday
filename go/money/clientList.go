package main

import (
	"fmt"
	"math/rand"
	"sync"
	"time"
)

var (
	guid int = 0
)

type ClientList struct {
	count    int
	clients  map[int]*Clt
	addQueue chan *Clt
	delQueue chan *Clt
	cntMutex *sync.Mutex
}

func NewClientList() *ClientList {
	l := &ClientList{0, make(map[int]*Clt), make(chan *Clt, 128), make(chan *Clt, 128), &sync.Mutex{}}
	go l.worker()
	return l
}

func (l *ClientList) Count() int {
	l.cntMutex.Lock()
	defer l.cntMutex.UnLock()
	return l.count
}

func (l *ClientList) AddClient(c *Clt) {
	l.addQueue <- c
}

func (l *ClientList) DelClient(c *Clt) {
	l.delQueue <- c
}

func (l *ClientList) worker() {
	rand.Seed(time.Now().UTC().UnixNano())
	for {
		select {
		case add := <-l.addQueue:
			l.count++
			guid++
			add.cid = guid
			l.clients[add.cid] = add
		case del := <-l.delQueue:
			l.count--
			delete(l.clients, del.cid)
		}
	}
}
