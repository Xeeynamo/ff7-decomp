package main

func main() {
	if err := build("config/us.yaml"); err != nil {
		panic(err)
	}
}
