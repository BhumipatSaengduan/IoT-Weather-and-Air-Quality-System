from fastapi import FastAPI
import pandas as pd
from pydantic import BaseModel
from pyspark.sql import SparkSession
from pyspark.sql.functions import avg, max, min, sum, when


SPARK_APPLICATION_NAME = "IoT Data Analysis"  # @param {type:"string"}
spark = SparkSession.builder.appName(SPARK_APPLICATION_NAME).getOrCreate()


class Report(BaseModel):
  temperature: float
  humidity: float
  gas_detected: int
  rain_detected: int


db = pd.DataFrame([Report(temperature=.0, humidity=.0,
                  gas_detected=0, rain_detected=0).model_dump()])
db.drop(db.index, inplace=True)


app = FastAPI()


@app.post("/data")
def receive_data(report: Report):
  report = report.model_dump()
  db.loc[len(db)] = report
  return {"status": "success", "received_data": report}


@app.get("/analyze")
def analyze_data():
  if len(db) < 1:
    return {"status": "error", "error": "No data available"}

  df = spark.createDataFrame(db)

  analysis = df.select(
    avg("temperature").alias("avg_temp"),
    avg("humidity").alias("avg_humidity"),
    sum(when(df["gas_detected"] == 1, 1).otherwise(0)).alias("gas_leaked_days"),
    sum(when(df["rain_detected"] == 0, 1).otherwise(0)).alias("dry_days")
  ).collect()[0]

  return {
    "Average Temperature (°C)": analysis["avg_temp"],
    "Average Humidity (%)": analysis["avg_humidity"],
    "Total Gas Leaked Days": analysis["gas_leaked_days"],
    "Total Dry Days": analysis["dry_days"]
  }
