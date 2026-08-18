FROM python:3.14-slim

WORKDIR /backend

COPY requirements.txt .

RUN pip install --no-cache-dir --upgrade -r requirements.txt

COPY . .

CMD ["uvicorn", "app.api:app", "--host", "0.0.0.0", "--port", "8000"]