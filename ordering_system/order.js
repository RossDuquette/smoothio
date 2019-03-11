const url = "smoothio-endpoint.com/order";
const data = {
    id: 123,
    order: 456
};

function sendOrderRequest() {
    alert('Order recieved.');

    $.post(url, data, function (data, status) {
        console.log('${status}');
    });
}