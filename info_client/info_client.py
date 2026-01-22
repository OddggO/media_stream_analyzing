from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route("/test", methods=["POST"])
def test():
    # 解析 JSON
    data = request.get_json(silent=True)

    # 后台打印
    if data is not None:
        print("Received JSON:", data)

    # 返回响应
    return jsonify("received message")


if __name__ == "__main__":
    # 仅用于本地测试
    app.run(host="0.0.0.0", port=12021, debug=True)
