package main

import (
    "fmt"
    "io"
    "net/http"
    "os"
)

func handleLog(w http.ResponseWriter, r *http.Request) {
    if r.Method != http.MethodPost {
        http.Error(w, "Only POST allowed", http.StatusMethodNotAllowed)
        return
    }

    body, err := io.ReadAll(r.Body)
    if err != nil {
        http.Error(w, "Failed to read body", http.StatusInternalServerError)
        return
    }
    defer r.Body.Close()

    f, err := os.OpenFile("incoming_log.txt", os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
    if err != nil {
        http.Error(w, "Failed to open log file", http.StatusInternalServerError)
        return
    }
    defer f.Close()

    f.Write(body)
    f.Write([]byte("\n"))

    fmt.Fprintln(w, "Log received.")
}

func main() {
    http.HandleFunc("/log", handleLog)
    fmt.Println("Сервер запущен на порту 5001, ожидает POST /log")
    http.ListenAndServe(":5001", nil)
}
