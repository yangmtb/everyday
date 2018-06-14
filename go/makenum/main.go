package main

import (
	"encoding/binary"
	"fmt"
	"math/rand"
	"os"
	"time"
)

var (
	garr []int64
)

func makenum() {
	f, err := os.OpenFile("numfile2", os.O_CREATE|os.O_RDWR, 0755)
	if nil != err {
		fmt.Println(err)
		return
	}
	defer f.Close()
	rand.Seed(time.Now().UnixNano())
	const thread = 7
	c := make(chan int, thread)
	k := 0
	for k < thread {
		go func(no int) {
			sl := rand.Intn(40)
			time.Sleep(time.Duration(sl) * time.Millisecond)
			len := 70000
			var buf = make([]byte, 8)
			var off int64
			step := len / 100
			for i := 0; i < len; i++ {
				if 0 == i%step {
					fmt.Println(no, " is working ", i/step, "%(", i, ")")
					rand.Seed(time.Now().UnixNano())
					f.Sync()
				}
				//binary.BigEndian.PutUint32(buf, uint32(i))
				binary.BigEndian.PutUint64(buf, uint64(i))
				off = rand.Int63n(1*1024*1024*1024 - 8)
				//if off > 249 && off < 251 {
				//	fmt.Println("off:", off)
				//}
				_, err = f.WriteAt(buf, off)
				if nil != err {
					fmt.Println(no, err)
					break
				}
				time.Sleep(thread * time.Millisecond * 10)
			}
			c <- no
		}(k)
		k++
	}
	k = 0
	for k < thread {
		no := <-c
		fmt.Println("done", no)
	}
}

func readnum() {
	f, err := os.OpenFile("numfile", os.O_CREATE|os.O_RDWR, 0755)
	if nil != err {
		fmt.Println(err)
		return
	}
	defer f.Close()
	len := 34217728
	var off int64
	var res uint64
	var buf = make([]byte, 8)
	for i := 0; i < len; i++ {
		_, err = f.Read(buf)
		if nil != err {
			fmt.Println(err)
			break
		}
		res = binary.BigEndian.Uint64(buf)
		//fmt.Println(int64(res))
		garr = append(garr, int64(res))
	}
	off++
	fmt.Println("last:", int64(res))
}

func main() {
	t0 := time.Now()
	makenum()
	//readnum()
	t1 := time.Since(t0)
	fmt.Println("gone ", t1)
	//fmt.Println("heap:")
	//heapSort(garr)
	//fmt.Println("guibing:")
	//mergeSort(garr)
	//fmt.Println("kuai:")
	//qsort(garr)
	t2 := time.Since(t0)
	fmt.Println("gone ", t2)
}
