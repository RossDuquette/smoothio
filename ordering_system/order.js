const url = "http://192.168.1.134:5000/enqueue";

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
    // Need to have at least one solid food and one liquid food selected.
    if ((data.solidDispenseOptions[0] || data.solidDispenseOptions[1] || data.solidDispenseOptions[2])
        && (data.liquidDispenseOptions[0] || data.liquidDispenseOptions[1] || data.liquidDispenseOptions[2])) {
        return true;
    }
    return false;
}

function sendOrderRequest() {
    var formData = stripFormData();
    if (!validateFormData(formData)) {
        alert("Invalid Order!")
        return;
    }

    $.post(url, formData, function (data, status) {
        console.log('${status}');
    });
    alert("Rquest Successful");
}