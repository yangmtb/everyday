package common

// Client Info
type CltInfo struct {
	//Cid    int    `json:"cid"`
	//IP     string `json:"ip"`
	CPU    int    `json:"cpu"`
	Mem    int    `json:"mem"`
	Status string `json:"status"`
	Tasks  []struct {
		Tid    int      `json:"tid"`
		Status string   `json:"status"`
		Start  int      `json:"start"`
		End    int      `json:"end"`
		Path   string   `json:"path"`
		Arg    []string `json:"arg"`
	} `json:"tasks"`
}
