from flask import Flask, jsonify, render_template 
import mysql.connector
from decimal import Decimal
from db_config import db 
import json

app = Flask(__name__, template_folder='view')


def get_db_connection():
    try : 
        conn = mysql.connector.connect(**db)
        return conn
    except mysql.connector.Error as err:
        print(f"Database tidak terkoneksi: {err}")
        return None

# ! Row Formating
def format_row(row):
    formated = {
        'idx' : row['id'],
        'suhu' : float(row['suhu']),
        'humidity' : float(row['humidity']),
        'kecerahan' : float(row['lux']),
        'timestamp' : row['timestamp'].strftime('%Y-%m-%d %H:%M:%S')
    }
    return formated

# ! Month Formating
def format_month_row(row):
    return {
        'month_year': row['month_year'],
    }


@app.route('/')
def home():
    return render_template('index.html')


@app.route('/api/sensor-data')
def get_sensor_data():
    conn = get_db_connection()
    
    if conn is None:
        return jsonify({'error': 'Database connection failed'}), 500
    
    cursor = conn.cursor(dictionary=True)
    output_json = {}

    try:
        # ! Fetch data suhu
        cursor.execute("SELECT MAX(suhu) as suhumax, MIN(suhu) as suhumin, AVG(suhu) as suhurata FROM data_sensor")
        stats = cursor.fetchone()

        output_json['suhumax'] = float(stats['suhumax'])
        output_json['suhumin'] = float(stats['suhumin'])
        output_json['suhurata'] = round(float(stats['suhurata']), 2) 

        # ! Fetch data suhuMax humidMax
        querry1 = "SELECT id, suhu, humidity, lux, timestamp FROM data_sensor WHERE suhu = (SELECT MAX(suhu) FROM data_sensor) AND humidity = (SELECT MAX(humidity) FROM data_sensor)"
        cursor.execute(querry1)
        max_rows = cursor.fetchall()
        output_json['nilai_suhu_max_humid_max'] = [format_row(row) for row in max_rows]
        
        # ! month year max
        querry2 = "SELECT DISTINCT CONCAT(MONTH(timestamp), '-', YEAR(timestamp)) AS month_year FROM data_sensor WHERE suhu = (SELECT MAX(suhu) FROM data_sensor) AND humidity = (SELECT MAX(humidity) FROM data_sensor)"
        cursor.execute(querry2)
        month_rows = cursor.fetchall() 
        output_json['month_year_max'] = [format_month_row(row) for row in month_rows] 

        # ! Exporting json
        try:
            with open('sensor_data_output.json', 'w') as f:
                json.dump(output_json, f, indent=4)
            print("Berhasil menyimpan data ke sensor_data_output.json")
        except Exception as e:
            print(f"Gagal menyimpan file JSON: {e}")
            pass
            
    except mysql.connector.Error as err:
        print(f"Error fetching suhu data: {err}")
        return jsonify({'error': 'Failed to fetch suhu data'}), 500 
    finally:
        cursor.close()
        conn.close()
    
    return jsonify(output_json)

if __name__ == '__main__':
    app.run(debug=True, port=5000)