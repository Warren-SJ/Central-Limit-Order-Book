import http.client
import json
import random
BASE_HOST = "localhost"
BASE_PORT = 8080
conn = http.client.HTTPConnection(BASE_HOST, BASE_PORT)
TICKERS = list(range(1, 16))
BASE_PRICES = {
    1: 15000, 2: 30000, 3: 12000, 4: 13000, 5: 20000,
    6: 25000, 7: 40000, 8: 14000, 9: 22000, 10: 9000,
    11: 35000, 12: 38000, 13: 7000, 14: 3000, 15: 5000,
}
NUM_ORDERS = 100000
BUYERS = list(range(1, 9))
SELLERS = list(range(9, 16))
def send_request(method, path, payload):
    headers = {"Content-Type": "application/json"}
    body = json.dumps(payload)
    try:
        # Reuses the existing open socket
        conn.request(method, path, body, headers)
        response = conn.getresponse()
        status = response.status
        response_body = response.read().decode("utf-8")
        return status, response_body
    except Exception as e:
        return None, str(e)
def add_order(stock, client, side, price, quantity):
    payload = {
        "stock": stock,
        "client": client,
        "side": side,
        "price": price,
        "quantity": quantity
    }
    return send_request("POST", "/api/order/add", payload)
def edit_order(order_id, price, quantity):
    payload = {
        "price": price,
        "quantity": quantity
    }
    return send_request("PATCH", f"/api/order/edit/{order_id}", payload)
if __name__ == "__main__":
    random.seed(42)
    print("=== Order Book API Simulation ===")
    successful_orders = 0
    submitted_orders = []
    start_time = time.time() if 'time' in globals() else 0 # Just for tracking
    import time
    start_time = time.time()
    for i in range(1, NUM_ORDERS + 1):
        stock = random.choice(TICKERS)
        side = random.choice(["BUY", "SELL"])
        client = random.choice(BUYERS) if side == "BUY" else random.choice(SELLERS)
        price = BASE_PRICES[stock] + random.randint(-250, 250)
        quantity = random.randint(5, 50)
        status, response = add_order(stock, client, side, price, quantity)
        if status == 200:
            successful_orders += 1
            submitted_orders.append({"id": i, "stock": stock, "side": side, "price": price, "quantity": quantity})
        else:
            print(f"  [-] Failed to add order {i}: Status {status}, Response: {response}")
    print(f"[+] Placed {successful_orders}/{NUM_ORDERS} orders in {time.time() - start_time:.2f} seconds.")
    if submitted_orders:
        print("\n[+] Simulating Order Replacements...")
        orders_to_edit = random.sample(submitted_orders, min(5, len(submitted_orders)))
        for order in orders_to_edit:
            new_price = order["price"] + random.choice([-100, 100])
            new_qty = max(5, order["quantity"] + random.randint(-2, 5))
            print(f"  Editing Order ID {order['id']}:")
            status, response = edit_order(order["id"], new_price, new_qty)
            print(f"    Result: Status {status}, Response: {response}")
    conn.close()
    