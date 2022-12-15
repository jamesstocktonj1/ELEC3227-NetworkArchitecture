package main


import (
	"fmt"
	"log"
	"time"
	"os"
	"go.bug.st/serial"
)

const (
	LOG_DIR = "../../data"
)

func main() {

	ports, err := serial.GetPortsList()
	if err != nil {
		log.Fatal(err)
	} else if len(ports) == 0 {
		log.Fatal("No serial devices found!")
	}


	serialMode := &serial.Mode {BaudRate: 9600,}
	serialPort, err := serial.Open(ports[0], serialMode)
	defer serialPort.Close()
	if err != nil {
		log.Fatal(err)
	}

	curTime := time.Now()
	fileName := fmt.Sprintf("%s/serial_log_%d_%d_%d.log", LOG_DIR, curTime.Hour(), curTime.Minute(), curTime.Second())
	
	loggingFile, err := os.OpenFile(fileName, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0666)
	if err != nil {
		log.Fatal(err)
	}

	serialLogger := log.New(loggingFile, "Serial: ", log.Lmicroseconds)

	SerialHandler(serialPort, serialLogger)
}


func CheckEOL(buffer string) int {

	for i, c := range buffer {
		if c == '\n' {
			return  i
		}
	}
	return -1
}

func SerialHandler(serialPort serial.Port, logFile *log.Logger) {

	serialAvailable := true

	temp := make([]byte, 100)
	lineBuffer := ""

	serialPort.ResetInputBuffer()

	for serialAvailable {

		n, err := serialPort.Read(temp)
		if err != nil {
			logFile.Println("Disconnect")
			fmt.Println("Disconnect")
			serialAvailable = false
		} else if n == 0 {
			logFile.Println("End of File")
			fmt.Println("End of File")
		} else {
			lineEnd := CheckEOL(string(temp[:n]))
			if lineEnd == -1 {
				lineBuffer += string(temp[:n])
			} else {
				lineData := lineBuffer + string(temp[:lineEnd])
				lineBuffer = string(temp[lineEnd+1:n])

				logFile.Println(lineData)
				fmt.Println(lineData)
			}
		}
	}
}