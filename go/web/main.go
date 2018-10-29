package main

import (
	"html/template"
	"net/http"

	"github.com/gin-gonic/gin"
)

var html = template.Must(template.New("https").Parse(`
<html>
<head>
  <title>Double Tetris</title>
</head>
<body>
  <h1 style="color:blue;">Welcome To Double Tetris.</h1>
</body>
</html>
`))

func main() {
	r := gin.Default()
	//r.Static("/assets", "./assets")
	r.SetHTMLTemplate(html)

	r.GET("/", func(c *gin.Context) {
		/*if pusher := c.Writer.Pusher(); pusher != nil {
			// use pusher.Push() to do server push
			if err := pusher.Push("/assets/app.js", nil); err != nil {
				log.Printf("Failed to push: %v", err)
			}
		}*/
		c.HTML(http.StatusOK, "https", gin.H{
			"status": "success",
		})
	})

	// Listen and Server in https://127.0.0.1:8080
	r.RunTLS("", "./key_web/server.pem", "./key_web/server.key")
}
