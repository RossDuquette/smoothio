const endpoint = "http://192.168.1.134:5000/enqueue";

function stripFormData() {
    var solidDispenseOptions = [false, false, false];
    var liquidDispenseOptions = [false, false, false];
    var secret = "";

    solidDispenseOptions[0] = document.getElementById("solid-dispense1").checked;
    solidDispenseOptions[1] = document.getElementById("solid-dispense2").checked;
    solidDispenseOptions[2] = document.getElementById("solid-dispense3").checked;

    liquidDispenseOptions[0] = document.getElementById("liquid-dispense1").checked;
    liquidDispenseOptions[1] = document.getElementById("liquid-dispense2").checked;
    liquidDispenseOptions[2] = document.getElementById("liquid-dispense3").checked;

    secret = document.getElementById("secret-code").value;

    // Serialize Dispense Options for easy string passing
    var solidStr = "";
    var liquidStr = "";
    for (var i = 0; i < 3; i++) {
        solidStr += solidDispenseOptions[i] ? "1" : "0";
        liquidStr += liquidDispenseOptions[i] ? "1" : "0";
    }

    return {
        solid: solidStr,
        liquid: liquidStr,
        secret: secret
    };
}

function validateFormData(data) {
    console.log(data);
    // Need to have at least one solid food and one liquid food selected.
    if (data.solid.includes("1") && data.liquid.includes("1") && data.secret.length > 0) {
        return true;
    }
    return false;
}

function formatParams(params) {
    return "?" + Object
        .keys(params)
        .map(function (key) {
            return key + "=" + encodeURIComponent(params[key])
        })
        .join("&")
}

function sendOrderRequest() {
    console.log("sendOrderRequest");
    var formData = stripFormData();
    if (!validateFormData(formData)) {
        alert("You must select at least 1 solid, 1 liquid, and provide a secret.")
        return;
    }

    // Backup HTTP GET request
    const xhr = new XMLHttpRequest();
    var url = endpoint + formatParams(formData)

    // $.get(url, function (responseText) {
    //     alert(responseText);
    // });

    $.ajax({
        type: "GET",
        url: url,
        dataType: 'jsonp', // necessary, because you're sending json from server
        cors: true,
        contentType: 'application/json',
        secure: true,
        headers: {
            'Access-Control-Allow-Origin': '*',
        },
        success: function (reponse) {  // response will catch within success functio
            console.log(response);
            alert("Success, making smoothie.");
        },
        error: function (request, error) {  // response will catch within success function
            console.log(error);
            alert("Failure, could not make smoothie.")
        }

    });

    // xhr.open("GET", url, true);
    // xhr.onreadystatechange = function () {
    //     if (xhr.readyState == XMLHttpRequest.DONE) {
    //         alert(xhr.responseText);
    //     }
    // }
    // xhr.send(null);

}

function setImageVisible(id, visible) {
    var img = document.getElementById(id);
    img.style.visibility = (visible ? 'visible' : 'hidden');
}

// Event Listeners
window.onload = function () {
    // Override so page doesn't refresh.
    $('#orderForm').submit(function () {
        // sendOrderRequest();
        return false;
    });

    // Checkbox Event Listeners
    var checboxImagePairs = [
        ["solid-dispense1", "img-strawberry"],
        ["solid-dispense2", "img-blueberry"],
        ["solid-dispense3", "img-banana"],
        ["liquid-dispense1", "img-orange"],
        ["liquid-dispense2", "img-cranberry"],
        ["liquid-dispense3", "img-yogurt"]
    ];

    checboxImagePairs.forEach(function (pair) {
        var checkbox = document.getElementById(pair[0]);
        checkbox.addEventListener('change', function () {
            setImageVisible(pair[1], this.checked);
        });
    });

    var submitButton = document.getElementById("submit-button");
    submitButton.addEventListener('click', function () {
        sendOrderRequest();
    });
};
