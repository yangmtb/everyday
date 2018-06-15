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
	const thread = 1
	c := make(chan int, thread)
	k := 0
	for k < thread {
		go func(no int) {
			sl := rand.Intn(40)
			fmt.Println(no, "will sleep ", sl)
			time.Sleep(time.Duration(sl) * time.Millisecond * 10)
			len := 800000000
			//con := 2
			//var buf = make([]byte, 8*con)
			var buf = make([]byte, 1*1024*1024*1024)
			var tmp = make([]byte, 8)
			var off int64 = 0
			step := len / 100
			for i := 0; i < len; i++ {
				if 0 == i%step {
					t := time.Now()
					fmt.Println(no, " is working   ", i/step, "%    ", i, "", t)
					rand.Seed(t.UnixNano())
					//f.Sync()
				}
				//binary.BigEndian.PutUint32(buf, uint32(i))
				//for j := 0; j < con; j++ {
				num := rand.Int63n(1 << 60)
				off = rand.Int63n(1*1024*1024*1024 - 8)
				binary.BigEndian.PutUint64(buf[off:], uint64(num))
				//off += 8
				//buf = append(buf, tmp...)
				//}
				//off = rand.Int63n(512*1024*1024 - 8)
				//time.Sleep(thread * time.Millisecond * 100)
			}
			fmt.Println(time.Now(), buf[:16], tmp)
			off = 0
			_, err = f.WriteAt(buf, off)
			fmt.Println(time.Now())
			if nil != err {
				fmt.Println(no, err)
			}
			c <- no
			return
		}(k)
		k++
	}
	k = 0
	for k < thread {
		fmt.Println("eeee", k)
		no := <-c
		fmt.Println("done", no)
		k++
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
