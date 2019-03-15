const url = "http://192.168.1.134:5000/enqueue";

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

    return {
        solid: solidDispenseOptions,
        liquid: liquidDispenseOptions,
        secret: secret
    };
}

function validateFormData(data) {
    console.log(data);
    // Need to have at least one solid food and one liquid food selected.
    if ((data.solid[0] || data.solid[1] || data.solid[2])
        && (data.liquid[0] || data.liquid[1] || data.liquid[2])
        && secret.length > 0) {
        return true;
    }
    return false;
}

function sendOrderRequest() {
    console.log("sendOrderRequest");
    var formData = stripFormData();
    if (!validateFormData(formData)) {
        alert("Invalid Order!")
        return;
    }

    // $.post(url, formData, function (data, status) {
    //     console.log('${status}');
    // });
    alert("Rquest Successful");
}

// Event Listeners
window.onload = function () {
    // Submit Event Listener
    $('#orderForm').submit(function () {
        // sendOrderRequest();
        return false;
    });

    // Checkbox Event Listeners
    var checkbox = document.getElementById("solid-dispense1");
    checkbox.addEventListener('change', function () {
        if (this.checked) {
            console.log("Clicked");
        } else {
            console.log("Unclicked");
        }
    });

    var submitButton = document.getElementById("submit-button");
    submitButton.addEventListener('click', function () {
        sendOrderRequest();
    });
};
