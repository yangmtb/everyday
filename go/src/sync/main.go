package main

import (
	"fmt"
	"os"
	"os/signal"
	"syscall"
	"time"
	"strings"

	"sync/pubsub"
)

func Producer(factor int, out chan<- int) {
	for i:=0;;i++{
		out <- factor
	}
}

func Consumer(in <- chan int) {
	for v:=range in {
		fmt.Println(v)
	}
}

func GenerateNatural() chan int {
	ch := make(chan int)
	go func() {
		for i:=2;;i++{
			ch <- i
		}
	}()
	return ch
}

func PrimeFilter(in <-chan int, prime int) chan int {
	out := make(chan int)
	go func() {
		for {
			if i:=<-in;i%prime != 0{
				out <-i
			}
		}
	}()
	return out
}

func main() {
	ch0 := GenerateNatural()
	for i:=0;i<100;i++{
		prime := <-ch0
		fmt.Printf("%v: %v\n",i+1,prime)
		ch0=PrimeFilter(ch0,prime)
	}
	return

	p := pubsub.NewPublisher(100*time.Millisecond, 10)
	defer p.Close()
	all := p.Subscribe()
	golang := p.SubscribeTopic(func(v interface{}) bool {
		if s, ok := v.(string); ok {
			return strings.Contains(s, "golang")
		}
		return false
	})
	p.Publish("helllo, world")
	p.Publish("helllo, golang")
	go func() {
		for msg := range all {
			fmt.Println("all:", msg)
		}
	}()
	go func() {
		for msg := range golang {
			fmt.Println("golang:", msg)
		}
	}()
	time.Sleep(3*time.Second)
	return
	ch := make(chan int, 64)
	go Producer(3,ch)
	go Producer(5,ch)
	go Consumer(ch)
	sig := make(chan os.Signal, 1)
	signal.Notify(sig, syscall.SIGINT, syscall.SIGTERM)
	fmt.Printf("done (%v)\n", <-sig)
}
