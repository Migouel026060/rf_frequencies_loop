#!/usr/bin/env python3
"""Serveur HTTP simple pour la maquette microfinance."""

import os
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

HOST = os.environ.get("HOST", "0.0.0.0")
PORT = int(os.environ.get("PORT", "8080"))
STATIC_DIR = Path(__file__).resolve().parent / "static"


class StaticHandler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(STATIC_DIR), **kwargs)


def run() -> None:
    with ThreadingHTTPServer((HOST, PORT), StaticHandler) as server:
        print(f"Serveur démarré sur http://{HOST}:{PORT}")
        server.serve_forever()


if __name__ == "__main__":
    run()
