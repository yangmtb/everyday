package main

import (
	"encoding/binary"
	"fmt"
	"math/rand"
	"os"
	"time"
)

func makenum(name string) {
	f, err := os.OpenFile(name, os.O_CREATE|os.O_WRONLY, 0644)
	if nil != err {
		fmt.Println(err)
		return
	}
	defer f.Close()
	rand.Seed(time.Now().UnixNano())
<<<<<<< HEAD
	const thread = 1
=======
	const thread = 5
>>>>>>> d7c9a25c377484bca1f55600e7211cdfb0766728
	c := make(chan int, thread)
	var buf = make([]byte, 1<<31)
	k := 0
	for k < thread {
		go func(no int) {
<<<<<<< HEAD
			sl := rand.Intn(40)
			fmt.Println(no, "will sleep ", sl)
			time.Sleep(time.Duration(sl) * time.Millisecond * 10)
			len := 800000000
			//con := 2
			//var buf = make([]byte, 8*con)
			var buf = make([]byte, 1*1024*1024*1024)
			var tmp = make([]byte, 8)
			var off int64 = 0
=======
			//sl := rand.Intn(40)
			//time.Sleep(time.Duration(sl) * time.Millisecond)
			time.Sleep(time.Duration(no+1) * thread * time.Millisecond * 10)
			len := 80000000
			var off int64
>>>>>>> d7c9a25c377484bca1f55600e7211cdfb0766728
			step := len / 100
			var fu func(b []byte, v uint64)
			if 0 == no%2 {
				fu = binary.BigEndian.PutUint64
			} else {
				fu = binary.LittleEndian.PutUint64
			}
			for i := 0; i < len; i++ {
				if 0 == i%step {
<<<<<<< HEAD
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
=======
					fmt.Println(no, "is working (", i/step, "%) ", i, "", buf[:16])
					rand.Seed(time.Now().UnixNano())
					//f.Sync()
				}
				//binary.BigEndian.PutUint32(buf, uint32(i))
				off = rand.Int63n(1*1024*1024*1024 - 8)
				nu := rand.Int63n(1 << 55)
				//binary.BigEndian.PutUint64(buf[off:], uint64(nu))
				fu(buf[off:], uint64(nu))
				//if off > 249 && off < 251 {
				//	fmt.Println("off:", off)
				//}
>>>>>>> d7c9a25c377484bca1f55600e7211cdfb0766728
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
<<<<<<< HEAD
=======
	}
	_, err = f.WriteAt(buf, 0)
	if nil != err {
		fmt.Println("write:", err)
>>>>>>> d7c9a25c377484bca1f55600e7211cdfb0766728
	}
}

func readArr(name string) (arr []int64) {
	f, err := os.OpenFile(name, os.O_RDONLY, 0644)
	if nil != err {
		fmt.Println(err)
		return
	}
	defer f.Close()
	var res uint64
	finfo, _ := f.Stat()
	all := finfo.Size()
	//all = all / 2
	step := all / 100
	var i int64
	for i < all {
		var buf = make([]byte, 8*1024)
		n, err := f.Read(buf)
		if nil != err {
			fmt.Println(i, err)
			break
		}
		i += int64(n)
		if 0 == i%step {
			fmt.Println("read", i/step, "%")
		}
		off := 0
		for off < n {
			res = binary.BigEndian.Uint64(buf[off:])
			off += 8
			arr = append(arr, int64(res))
		}
	}
	return arr
}

func writeArr(name string, arr []int64) {
	f, err := os.OpenFile(name, os.O_CREATE|os.O_WRONLY, 0644)
	if nil != err {
		fmt.Println(err)
		return
	}
	defer f.Close()
	//buf := make([]byte, 8*len(arr))
	//off := 0
	sz := 1 << 26
	var aoff int64
	buf := make([]byte, sz)
	off := 0
	for _, v := range arr {
		if off < sz {
			binary.BigEndian.PutUint64(buf[off:], uint64(v))
			off += 8
		} else if off == sz {
			off = 0
			//go func(k int) {
			//fmt.Println("write:", k)
			_, err = f.WriteAt(buf, aoff)
			if nil != err {
				fmt.Println(err)
			}
			aoff += int64(sz)
			//fmt.Println("write ok")
			time.Sleep(time.Second)
		} else {
			fmt.Println("boom")
			break
		}
		//}(k)
	}
}

func main() {
	t0 := time.Now()
	//makenum("rand.num")
	garr := readArr("rand.num")
	t1 := time.Since(t0)
	fmt.Println(len(garr), "read time: ", t1)
	heapSort(garr)
	//fmt.Println("guibing:")
	//mergeSort(garr)
	//fmt.Println("kuai:")
	//qsort(garr)
	t2 := time.Since(t0)
	fmt.Println(len(garr), "gone ", t2)
	fmt.Println("sort:", len(garr))
	t3 := time.Now()
	fmt.Println("begin write", len(garr))
	writeArr("sort.num", garr)
	fmt.Println(time.Since(t3))
	return
	var minNum int64
	var maxNum int64
	var ca int64
	for k, v := range garr {
		//if 0 == k%50 {
		//time.Sleep(1 * time.Second)
		//}
		if k+1 == len(garr) {
			fmt.Println("done:", k, v)
		} else if 0 == k {
			ca = garr[k+1] - v
			maxNum = ca
			minNum = 1 << 62
		} else {
			ca = garr[k+1] - v
			if ca > maxNum {
				maxNum = ca
				fmt.Println(k, v, ca, maxNum, minNum)
			}
			if ca < minNum {
				minNum = ca
				fmt.Println(k, v, ca, maxNum, minNum)
			}
		}
	}
	fmt.Println("min:", minNum, "max:", maxNum)
}
