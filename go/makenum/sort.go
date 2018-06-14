package main

// dui
func heap(arr []int64, i, end int) {
	l := 2*i + 1
	if l > end {
		return
	}
	n := l
	r := 2*i + 2
	if r <= end && arr[r] > arr[l] {
		n = r
	}
	if arr[i] > arr[n] {
		return
	}
	arr[n], arr[i] = arr[i], arr[n]
	heap(arr, n, end)
}

func heapSort(arr []int64) {
	m := len(arr)
	s := m / 2
	for i := s; i > -1; i-- {
		heap(arr, i, m-1)
	}
	for i := m - 1; i > 0; i-- {
		arr[i], arr[0] = arr[0], arr[i]
		heap(arr, 0, i-1)
	}
}

// guibing
func merge(left, right []int64) []int64 {
	tmp := make([]int64, 0)
	i, j := 0, 0
	for i < len(left) && j < len(right) {
		if left[i] < right[j] {
			tmp = append(tmp, left[i])
			i++
		} else {
			tmp = append(tmp, right[j])
			j++
		}
	}
	tmp = append(tmp, left[i:]...)
	tmp = append(tmp, right[j:]...)
	return tmp
}

func mergeSort(arr []int64) []int64 {
	n := len(arr)
	if n < 2 {
		return arr
	}
	key := n / 2
	left := mergeSort(arr[0:key])
	right := mergeSort(arr[key:])
	return merge(left, right)
}

// kuai
func qsort(arr []int64) {
	if len(arr) <= 1 {
		return
	}
	mid := arr[0]
	head, tail := 0, len(arr)-1
	for i := 1; i < tail; {
		if arr[i] > mid {
			arr[i], arr[tail] = arr[tail], arr[i]
			tail--
		} else {
			arr[i], arr[head] = arr[head], arr[i]
			head++
			i++
		}
	}
	qsort(arr[:head])
	qsort(arr[head+1:])
}
