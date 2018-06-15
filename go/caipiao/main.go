package main

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"strconv"
	"strings"
	"time"

	"github.com/PuerkitoBio/goquery"
)

func ExampleScrape() {
	// Request the HTML page.
	//res, err := http.Get("https://datachart.500.com/ssq/history/history.shtml")
	res, err := http.Get("https://kj.cjcp.com.cn/ssq/index.php?topage=1")
	if err != nil {
		log.Fatal(err)
	}
	defer res.Body.Close()
	if res.StatusCode != 200 {
		log.Fatalf("status code error: %d %s", res.StatusCode, res.Status)
	}

	// Load the HTML document
	doc, err := goquery.NewDocumentFromReader(res.Body)
	if err != nil {
		log.Fatal(err)
	}

	// Find the review items
	doc.Find(".qgkj_table").Each(func(i int, s *goquery.Selection) {
		// For each item found, get the band and title
		band := s.Find(".t_cfont2")
		title := s.Find(".t_cfont4")
		fmt.Printf("Review %d: %v %v\n", i, band.Text(), title.Text())
	})
}

type caipiaoInfo struct {
	No         int       `bson:"no"`
	Date       time.Time `bson:"date"`
	Red        []string  `bson:"red"`
	Blue       string    `bson:"blue"`
	Order      []string  `bson:"order"`
	TouZhu     int64     `bson:"touzhu"`
	JiangChi   int64     `bson:"jiangchi"`
	OneCount   int       `bson:"onecount"`
	OneMoney   int       `bson:"onemoney"`
	TwoCount   int       `bson:"twocount"`
	TwoMoney   int       `bson:"twomoney"`
	ThreeCount int       `bson:"threecount"`
	ThreeMoney int       `bson:"threemoney"`
	FourCount  int       `bson:"fourcount"`
	FourMoney  int       `bson:"fourmoney"`
	FiveCount  int       `bson:"fivecount"`
	FiveMoney  int       `bson:"fivemoney"`
	SixCount   int       `bson:"sixcount"`
	SixMoney   int       `bson:"sixmoney"`
}

var (
	cps []caipiaoInfo
	r1  []int
	r2  []int
	r3  []int
	r4  []int
	r5  []int
	r6  []int
	b1  []int
)

func readCaipiao() {
	f, err := os.Open("ssq_DESC.TXT")
	if err != nil {
		log.Fatal(err)
	}
	defer f.Close()
	buf := bufio.NewReader(f)
	i := 0
	line, is, err := buf.ReadLine()
	if err != nil {
		log.Fatal(err)
	}
	/*dialInfo := &mgo.DialInfo{
		Addrs:     []string{"159.65.11.201"},
		Timeout:   time.Second * 8,
		Database:  "love",
		Username:  "wdq",
		Password:  "y1218",
		PoolLimit: 4096,
	}
	session, err := mgo.DialWithInfo(dialInfo)
	if err != nil {
		log.Fatal(err)
	}
	defer session.Close()
	db := session.DB("love")
	if db == nil {
		log.Fatal("kj")
	}
	caipiao := db.C("caipiao")*/
	for {
		line, is, err = buf.ReadLine()
		if err != nil && err != io.EOF {
			log.Fatal(is, err)
		} else if err == io.EOF {
			break
		}
		all := strings.Split(string(line), " ")
		var c caipiaoInfo
		c.No, _ = strconv.Atoi(all[0])
		c.Date, _ = time.Parse("2006-01-02", all[1])
		c.Red = all[2:8]
		c.Blue = all[8]
		t, _ := strconv.Atoi(all[8])
		b1 = append(b1, t)
		c.Order = append(c.Order, all[14], all[13], all[12], all[11], all[10], all[9])
		t, _ = strconv.Atoi(all[14])
		r1 = append(r1, t)
		t, _ = strconv.Atoi(all[13])
		r2 = append(r2, t)
		t, _ = strconv.Atoi(all[12])
		r3 = append(r3, t)
		t, _ = strconv.Atoi(all[11])
		r4 = append(r4, t)
		t, _ = strconv.Atoi(all[10])
		r5 = append(r5, t)
		t, _ = strconv.Atoi(all[9])
		r6 = append(r6, t)
		c.TouZhu, _ = strconv.ParseInt(all[15], 10, 64)
		c.JiangChi, _ = strconv.ParseInt(all[16], 10, 64)
		c.OneCount, _ = strconv.Atoi(all[17])
		c.OneMoney, _ = strconv.Atoi(all[18])
		c.TwoCount, _ = strconv.Atoi(all[19])
		c.TwoMoney, _ = strconv.Atoi(all[20])
		c.ThreeCount, _ = strconv.Atoi(all[21])
		c.ThreeMoney, _ = strconv.Atoi(all[22])
		c.FourCount, _ = strconv.Atoi(all[23])
		c.FourMoney, _ = strconv.Atoi(all[24])
		c.FiveCount, _ = strconv.Atoi(all[25])
		c.FiveMoney, _ = strconv.Atoi(all[26])
		c.SixCount, _ = strconv.Atoi(all[27])
		c.SixMoney, _ = strconv.Atoi(all[28])
		cps = append(cps, c)
		/*err = caipiao.Insert(&c)
		if err != nil {
			log.Fatal(i, err)
		}*/
		i++
	}
}

func avg() {
	r1s, r2s, r3s, r4s, r5s, r6s, bs := 0, 0, 0, 0, 0, 0, 0
	for _, v := range cps {
		t, _ := strconv.Atoi(v.Order[0])
		r1s += t
		t, _ = strconv.Atoi(v.Order[1])
		r2s += t
		t, _ = strconv.Atoi(v.Order[2])
		r3s += t
		t, _ = strconv.Atoi(v.Order[3])
		r4s += t
		t, _ = strconv.Atoi(v.Order[4])
		r5s += t
		t, _ = strconv.Atoi(v.Order[5])
		r6s += t
		t, _ = strconv.Atoi(v.Blue)
		bs += t
	}
	fmt.Println("r1:", r1s/len(cps))
	fmt.Println(r1s, "/", len(cps), "=", r1s/len(cps))
	fmt.Println("r2:", r2s/len(cps))
	fmt.Println(r2s, "/", len(cps), "=", r2s/len(cps))
	fmt.Println("r3:", r3s/len(cps))
	fmt.Println(r3s, "/", len(cps), "=", r3s/len(cps))
	fmt.Println("r4:", r4s/len(cps))
	fmt.Println(r4s, "/", len(cps), "=", r4s/len(cps))
	fmt.Println("r5:", r5s/len(cps))
	fmt.Println(r5s, "/", len(cps), "=", r5s/len(cps))
	fmt.Println("r6:", r6s/len(cps))
	fmt.Println(r6s, "/", len(cps), "=", r6s/len(cps))
	fmt.Println("b1:", bs/len(cps))
	fmt.Println(bs, "/", len(cps), "=", bs/len(cps))
}

func avg2() {
	r1s, r2s, r3s, r4s, r5s, r6s, b1s := 0, 0, 0, 0, 0, 0, 0
	for _, v := range r1 {
		r1s += v
	}
	for _, v := range r2 {
		r2s += v
	}
	for _, v := range r3 {
		r3s += v
	}
	for _, v := range r4 {
		r4s += v
	}
	for _, v := range r5 {
		r5s += v
	}
	for _, v := range r6 {
		r6s += v
	}
	for _, v := range b1 {
		b1s += v
	}
	fmt.Println("r1:", r1s, "/ 2273 =", r1s/2273)
	fmt.Println("r2:", r2s, "/ 2273 =", r2s/2273)
	fmt.Println("r3:", r3s, "/ 2273 =", r3s/2273)
	fmt.Println("r4:", r4s, "/ 2273 =", r4s/2273)
	fmt.Println("r5:", r5s, "/ 2273 =", r5s/2273)
	fmt.Println("r6:", r6s, "/ 2273 =", r6s/2273)
	fmt.Println("b1:", b1s, "/ 2273 =", b1s/2273)
}

func tongji(arr []int) (res []int) {
	res = make([]int, 33)
	for _, v := range arr {
		switch v {
		case 1:
			res[0]++
		case 2:
			res[1]++
		case 3:
			res[2]++
		case 4:
			res[3]++
		case 5:
			res[4]++
		case 6:
			res[5]++
		case 7:
			res[6]++
		case 8:
			res[7]++
		case 9:
			res[8]++
		case 10:
			res[9]++
		case 11:
			res[10]++
		case 12:
			res[11]++
		case 13:
			res[12]++
		case 14:
			res[13]++
		case 15:
			res[14]++
		case 16:
			res[15]++
		case 17:
			res[16]++
		case 18:
			res[17]++
		case 19:
			res[18]++
		case 20:
			res[19]++
		case 21:
			res[20]++
		case 22:
			res[21]++
		case 23:
			res[22]++
		case 24:
			res[23]++
		case 25:
			res[24]++
		case 26:
			res[25]++
		case 27:
			res[26]++
		case 28:
			res[27]++
		case 29:
			res[28]++
		case 30:
			res[29]++
		case 31:
			res[30]++
		case 32:
			res[31]++
		case 33:
			res[32]++
		}
	}
	return
}

func show(arr []int) {
	for k, v := range arr {
		if 10 == k || 20 == k || 30 == k {
			fmt.Println()
		}
		fmt.Printf(" %2d:%2d", k+1, v)
	}
	fmt.Println()
}

// 24(46)     15(55)       33(46)|8(51)        33(47)|2(52)          17(53)          29(48)    8(123)
func main() {
	readCaipiao()
	show(tongji(b1))
	//fmt.Println(tongji(r1))
	//fmt.Println(tongji(r2))
	//avg2()
	fmt.Println("done")
}
