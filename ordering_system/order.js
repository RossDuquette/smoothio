const url = "smoothio-endpoint.com/order";

function stripFormData() {
    var solidDispenseOptions = [false, false, false];
    var liquidDispenseOptions = [false, false, false];

    solidDispenseOptions[0] = document.getElementById("solid-dispense1").checked;
    solidDispenseOptions[1] = document.getElementById("solid-dispense2").checked;
    solidDispenseOptions[2] = document.getElementById("solid-dispense3").checked;

    liquidDispenseOptions[0] = document.getElementById("liquid-dispense1").checked;
    liquidDispenseOptions[1] = document.getElementById("liquid-dispense2").checked;
    liquidDispenseOptions[2] = document.getElementById("liquid-dispense3").checked;

    return {
        solid: solidDispenseOptions,
        liquid: liquidDispenseOptions
    };
}

function validateFormData(data) {
    // TODO: Ensure that a request contains valid parameters (ex. minimum selection criteria)
    return true;
}

function sendOrderRequest() {
    var formData = stripFormData();
    if (!validateFormData(formData)) {
        alert("Invalid Order!")
        return;
    }

    alert("Attempting Request");
    $.post(url, formData, function (data, status) {
        console.log('${status}');
    });
    alert("Rquest Successful");
}