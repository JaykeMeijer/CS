/*
 * Rebuild the list of free rooms, by destroying the current and building it
 * again.
 */
function rebuildList() {
    var list = document.getElementById("list");

    if(list.hasChildNodes())
        while (list.childNodes.length > 0)
            list.removeChild(list.firstChild);
    buildList();
}

/* Build the list of free rooms. */
function buildList() {
    var req = new XMLHttpRequest();
    var params = "request=list";
    post(req, params, function() {
            if (req.readyState == 4 && req.status == 200) {
                var data = JSON.parse(req.responseText);
                var list = document.getElementById("list");
                console.log(data);

                // For each room type: print a line
                for (room in data['rooms']) {
                    var curr = data['rooms'][room];
                    var string = "Room of type " + curr['type'] + ", €" +
                                 curr['price'] + ". Available number of rooms: "
                                 + curr['free'] +
                                 ". ";

                    // Create the DOM objects
                    var li = document.createElement("li");
                    var a = document.createElement("a");
                    a.setAttribute("href", "#");
                    a.setAttribute("onClick", "createBookPopup(" + 
                        curr['type'] + ")");
                    a.appendChild(document.createTextNode("Book"));
                    li.appendChild(document.createTextNode(string));
                    li.appendChild(a);
                    list.appendChild(li);
                }
            }
        });
}

/* Create the popup for booking a room. */
function createBookPopup(type) {
    var element = document.getElementById("book");
    document.getElementById("typeHolder").setAttribute("value", type);
    element.setAttribute("style", "display: block");
}

/* Hide the book popup. */
function cancel() {
    var element = document.getElementById("book");
    element.setAttribute("style", "display: none");
}

/* Perform the submitting of the 'form'. */
function submitForm() {
    var type = document.getElementById("typeHolder").value;
    var name = document.getElementById("pname").value;
    var req = new XMLHttpRequest();
    var params = "request=book&type=" + type + "&name=" + name;
    post(req, params,   function() {
                            if (req.readyState == 4 && req.status == 200)
                                rebuildList();
                        });
    cancel();
}

/* Rebuild the list of guests, by destroying and building. */
function rebuildGuests() {
    var guest = document.getElementById("guests");

    if(guests.hasChildNodes())
        while (guests.childNodes.length > 0)
            guests.removeChild(guests.firstChild);
    buildGuests();
}

/* Build the list of guests in the hotel. */
function buildGuests() {
    var req = new XMLHttpRequest();
    var params = "request=guests";
    post(req, params, function() {
            if (req.readyState == 4 && req.status == 200) {
                var data = JSON.parse(req.responseText);
                var list = document.getElementById("guests");
                for (room in data['guests']) {
                    var curr = data['guests'][room];
                    if(curr == "")
                        return;
                    var li = document.createElement("li");
                    li.appendChild(document.createTextNode(curr));
                    list.appendChild(li);
                }
            }
        });
}

/* Perform a POST request to the server, and execute 'handler' when done. */
function post(req, params, handler) {
    req.open("POST", "handleRequest.php", true);
    req.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    req.setRequestHeader("Content-length", params.length);
    req.setRequestHeader("Connection", "close");

    req.onreadystatechange = handler;
    req.send(params);
}
