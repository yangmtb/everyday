package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"os/exec"
	"reflect"
	"sync"
	"time"
)

type data struct {
	sync.Mutex
}

func (d *data) test(s string) {
	d.Lock()
	defer d.Unlock()
	for i := 0; i < 5; i++ {
		println(s, i)
		time.Sleep(time.Second)
	}
}

func testLock() {
	var wg sync.WaitGroup
	wg.Add(2)

	var d data
	go func() {
		defer wg.Done()
		d.test("read")
	}()
	go func() {
		defer wg.Done()
		d.test("write")
	}()
	wg.Wait()
}

type X int

func testReflect() {
	var a X = 100
	var d data
	t := reflect.TypeOf(d)
	fmt.Println(t, t.Name(), t.Kind(), a)
	aa := reflect.ArrayOf(10, reflect.TypeOf(byte(0)))
	m := reflect.MapOf(reflect.TypeOf(""), reflect.TypeOf(0))
	fmt.Println(aa, m)
}

func testCPU() {
	go func() {
		for {

		}
	}()
	time.Sleep(30 * time.Second)
}

func testdo() {
	cmd := exec.Command("bash", "echo.sh")
	//cmd := exec.Command("ls", "-a", "-l")
	stdout, _ := cmd.StdoutPipe()
	defer stdout.Close()
	if err := cmd.Start(); nil != err {
		log.Fatal(err)
	}
	op, err := ioutil.ReadAll(stdout)
	if nil != err {
		log.Fatal(err)
	}
	fmt.Println("done", string(op))
}

func main() {
	//testReflect()
	//testCPU()
	testdo()
	return
}
