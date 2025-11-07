package main

func main() {
	if err := build("/home/xeeynamo/repo/ff7/config/us.yaml"); err != nil {
		panic(err)
	}
}
