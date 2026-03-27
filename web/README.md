# Serveur microfinance (maquette)

## Lancer le serveur

```bash
python3 web/server.py
```

Le serveur utilise par défaut `0.0.0.0:8080`.

Vous pouvez personnaliser:

```bash
HOST=127.0.0.1 PORT=8099 python3 web/server.py
```

## Fichiers

- `web/server.py` : serveur HTTP statique.
- `web/static/index.html` : page avec la mise en page microfinance.
- `web/static/styles.css` : styles de la page.
