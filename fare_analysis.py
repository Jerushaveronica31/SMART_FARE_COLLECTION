import streamlit as st
import requests
import pandas as pd
import random

#ப்பஜீஜ் டைட்டில் மற்றும் லேஅவுட் அமைப்பு
st.set_page_config(page_title="Smart Fare Collection Dashboard", page_icon="🚆", layout="wide")

st.title("🚆 Smart Fare Collection & Analytics Dashboard")
st.markdown("### 30 Users Live Transit Monitoring System")

firebase_url = "https://smart-fare-collection-default-rtdb.firebaseio.com/fare_logs.json"

# பட்டன்: 30 தனித்தனி யூசர் டேட்டாவை ஃபைர்பேஸில் உருவாக்க
if st.button("🔄 Generate 30 Unique Users Data"):
    statuses = ["Check-in", "Check-out"]
    for i in range(1, 31):
        card_id = f"USER-CARD-{random.randint(1000, 9999)}"
        status = random.choice(statuses)
        payload = {"cardUID": card_id, "status": status}
        requests.post(firebase_url, json=payload)
    st.success("30 வெவ்வேறு பயனர்களின் தரவுகள் வெற்றிகரமாக உருவாக்கப்பட்டுவிட்டன!")

# டேட்டாவை ஃபெட்ச் செய்து டேஷ்போர்டில் காட்டுதல்
response = requests.get(firebase_url)

if response.status_code == 200:
    data = response.json()
    if data is not None:
        records = []
        for key, value in data.items():
            if isinstance(value, dict):
                records.append(value)
        
        if len(records) > 0:
            df = pd.DataFrame(records)
            
            # டாப் மெட்ரிக்ஸ் கார்டுகள் (Metrics Cards)
            col1, col2, col3 = st.columns(3)
            col1.metric("Total Transit Logs", len(df))
            col2.metric("Check-ins", len(df[df['status'] == 'Check-in']))
            col3.metric("Check-outs", len(df[df['status'] == 'Check-out']))
            
            st.markdown("---")
            
            # கிராஃப்கள் மற்றும் டேபிள் அமைப்பு
            c1, c2 = st.columns(2)
            
            with c1:
                st.subheader("📊 Status Distribution Chart")
                status_counts = df['status'].value_counts()
                st.bar_chart(status_counts)
                
            with c2:
                st.subheader("👥 Live 30 Users Records Table")
                st.dataframe(df, use_container_width=True)
        else:
            st.warning("தற்பொழுது தரவுகள் எதுவும் இல்லை!")
    else:
        st.warning("டேட்டாபேஸ் காலியாக உள்ளது!")
else:
    st.error("ஃபைர்பேஸிலிருந்து தரவுகளை எடுப்பதில் பிழை ஏற்பட்டது.")