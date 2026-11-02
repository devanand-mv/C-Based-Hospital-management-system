/**
 * TRUECARE HOSPITAL MANAGEMENT SYSTEM - FRONTEND ENGINE
 * Connects directly to the native C backend (main.c) via HTTP REST endpoints.
 * All data storage, structures, arrays, and calculations are executed in C.
 */

// Global state holding records received from C
let patients = [];
let doctors = [];
let appointments = [];
let bills = [];

const isHostedByC = window.location.protocol.startsWith("http");

// ============================================================================
// INITIALIZATION
// ============================================================================
document.addEventListener("DOMContentLoaded", () => {
  setupNavigation();
  updateLiveDate();
  loadData();
});

function loadData() {
  if (isHostedByC) {
    // Fetch live from C backend arrays
    fetch("/api/data")
      .then(res => res.json())
      .then(data => {
        patients = data.patients || [];
        doctors = data.doctors || [];
        appointments = data.appointments || [];
        bills = data.bills || [];
        renderAll();
      })
      .catch(err => {
        console.warn("Could not connect to C backend, using offline fallback:", err);
        fallbackLocalStorage();
      });
  } else {
    fallbackLocalStorage();
  }
}

function fallbackLocalStorage() {
  const p = localStorage.getItem("truecare_patients") || localStorage.getItem("carewell_patients");
  const d = localStorage.getItem("truecare_doctors") || localStorage.getItem("carewell_doctors");
  const a = localStorage.getItem("truecare_appointments") || localStorage.getItem("carewell_appointments");
  const b = localStorage.getItem("truecare_bills") || localStorage.getItem("carewell_bills");

  patients = p ? JSON.parse(p) : [
    { id: 101, name: "Rahul Sharma", age: 32, gender: "Male", phone: "9876543210", disease: "Viral Fever & Fatigue" },
    { id: 102, name: "Priya Patel", age: 27, gender: "Female", phone: "9823456789", disease: "Migraine & Tension" },
    { id: 103, name: "Amit Verma", age: 54, gender: "Male", phone: "9123456780", disease: "Hypertension & Diabetes" },
    { id: 104, name: "Sneha Kulkarni", age: 41, gender: "Female", phone: "9988776655", disease: "Orthopedic Knee Pain" }
  ];

  doctors = d ? JSON.parse(d) : [
    { id: 201, name: "Aditi Roy", specialization: "Cardiology", phone: "9845011223" },
    { id: 202, name: "Vikram Mehta", specialization: "Neurology", phone: "9819022334" },
    { id: 203, name: "Rajesh Nair", specialization: "Orthopedics", phone: "9769033445" },
    { id: 204, name: "Sunita Rao", specialization: "General Medicine", phone: "9821044556" }
  ];

  appointments = a ? JSON.parse(a) : [
    { id: 301, patientId: 101, patientName: "Rahul Sharma", doctorId: 204, doctorName: "Sunita Rao", date: "2026-11-05" },
    { id: 302, patientId: 102, patientName: "Priya Patel", doctorId: 202, doctorName: "Vikram Mehta", date: "2026-11-06" },
    { id: 303, patientId: 104, patientName: "Sneha Kulkarni", doctorId: 203, doctorName: "Rajesh Nair", date: "2026-11-07" }
  ];

  bills = b ? JSON.parse(b) : [
    { id: 401, patientId: 101, patientName: "Rahul Sharma", consultation: 500, medicine: 650, treatment: 250, total: 1400, date: "2026-11-02" },
    { id: 402, patientId: 103, patientName: "Amit Verma", consultation: 600, medicine: 1200, treatment: 800, total: 2600, date: "2026-11-02" }
  ];

  renderAll();
}

function updateLiveDate() {
  const el = document.getElementById("liveDate");
  const now = new Date();
  const options = { weekday: "short", year: "numeric", month: "short", day: "numeric" };
  el.textContent = now.toLocaleDateString("en-US", options);
}

// ============================================================================
// NAVIGATION & TABS
// ============================================================================
function setupNavigation() {
  const navItems = document.querySelectorAll(".nav-item");
  navItems.forEach(item => {
    item.addEventListener("click", () => {
      const tab = item.getAttribute("data-tab");
      switchTab(tab);
    });
  });
}

function switchTab(tabId) {
  document.querySelectorAll(".nav-item").forEach(item => {
    if (item.getAttribute("data-tab") === tabId) {
      item.classList.add("active");
    } else {
      item.classList.remove("active");
    }
  });

  document.querySelectorAll(".tab-view").forEach(view => {
    view.classList.remove("active");
  });

  const targetView = document.getElementById(`view-${tabId}`);
  if (targetView) targetView.classList.add("active");

  const titleMap = {
    dashboard: { title: "Dashboard", sub: "Hospital activity overview and real-time statistics" },
    patients: { title: "Patient Management", sub: "Register, monitor, search and manage patient profiles" },
    doctors: { title: "Doctor Management", sub: "Hospital clinical staff and specialist directory" },
    appointments: { title: "Appointment Management", sub: "Schedule and manage doctor-patient consultations" },
    billing: { title: "Billing & Invoices", sub: "Itemized fee calculation and receipt generation" }
  };

  if (titleMap[tabId]) {
    document.getElementById("pageTitle").textContent = titleMap[tabId].title;
    document.getElementById("pageSubtitle").textContent = titleMap[tabId].sub;
  }

  if (tabId === "patients") renderPatientsTable();
  if (tabId === "doctors") renderDoctorsTable();
  if (tabId === "appointments") renderAppointmentsTable();
  if (tabId === "billing") renderBillingTable();
  if (tabId === "dashboard") renderDashboard();
}

// ============================================================================
// RENDER VIEWS
// ============================================================================
function renderAll() {
  updateBadges();
  renderDashboard();
  renderPatientsTable();
  renderDoctorsTable();
  renderAppointmentsTable();
  renderBillingTable();
}

function updateBadges() {
  document.getElementById("badgePatients").textContent = patients.length;
  document.getElementById("badgeDoctors").textContent = doctors.length;
  document.getElementById("badgeAppointments").textContent = appointments.length;
  document.getElementById("badgeBills").textContent = bills.length;
}

// ============================================================================
// 1. DASHBOARD
// ============================================================================
function renderDashboard() {
  document.getElementById("dashTotalPatients").textContent = patients.length;
  document.getElementById("dashTotalDoctors").textContent = doctors.length;
  document.getElementById("dashTotalAppointments").textContent = appointments.length;

  const totalRev = bills.reduce((acc, curr) => acc + (parseFloat(curr.total) || 0), 0);
  document.getElementById("dashTotalRevenue").textContent = `Rs. ${totalRev.toLocaleString('en-IN', { minimumFractionDigits: 2 })}`;

  const recentPatientsDiv = document.getElementById("dashRecentPatients");
  recentPatientsDiv.innerHTML = "";
  const latestPatients = patients.slice(-4).reverse();

  if (latestPatients.length === 0) {
    recentPatientsDiv.innerHTML = `<p style="color: var(--text-muted); font-size: 13px;">No patients registered yet.</p>`;
  } else {
    latestPatients.forEach(p => {
      const item = document.createElement("div");
      item.className = "mini-item";
      item.innerHTML = `
        <div class="mini-item-left">
          <div class="mini-avatar">${p.name.charAt(0)}</div>
          <div>
            <div class="mini-name">${p.name} <span class="badge badge-id">#${p.id}</span></div>
            <div class="mini-meta">${p.age} yrs &bull; ${p.gender} &bull; ${p.disease}</div>
          </div>
        </div>
        <span class="badge badge-specialty">${p.phone}</span>
      `;
      recentPatientsDiv.appendChild(item);
    });
  }

  const recentApptDiv = document.getElementById("dashRecentAppointments");
  recentApptDiv.innerHTML = "";
  const latestAppts = appointments.slice(-4).reverse();

  if (latestAppts.length === 0) {
    recentApptDiv.innerHTML = `<p style="color: var(--text-muted); font-size: 13px;">No scheduled appointments.</p>`;
  } else {
    latestAppts.forEach(a => {
      const item = document.createElement("div");
      item.className = "mini-item";
      item.innerHTML = `
        <div class="mini-item-left">
          <div class="mini-avatar" style="background: var(--teal-light); color: var(--teal-dark);">&#x2713;</div>
          <div>
            <div class="mini-name">${a.patientName}</div>
            <div class="mini-meta">Dr. ${a.doctorName} &bull; Appt #${a.id}</div>
          </div>
        </div>
        <span class="badge badge-date">${a.date}</span>
      `;
      recentApptDiv.appendChild(item);
    });
  }
}

// ============================================================================
// 2. PATIENT CRUD
// ============================================================================
function renderPatientsTable(list = patients) {
  const tbody = document.getElementById("patientTableBody");
  tbody.innerHTML = "";

  if (list.length === 0) {
    tbody.innerHTML = `<tr><td colspan="7" style="text-align:center; padding: 25px; color: var(--text-muted);">No matching patients found.</td></tr>`;
    return;
  }

  list.forEach(p => {
    const tr = document.createElement("tr");
    tr.innerHTML = `
      <td><span class="badge badge-id">#${p.id}</span></td>
      <td><strong>${p.name}</strong></td>
      <td>${p.age}</td>
      <td>${p.gender}</td>
      <td>${p.phone}</td>
      <td><span class="badge badge-specialty">${p.disease}</span></td>
      <td class="text-right">
        <button class="btn btn-danger-outline" onclick="deletePatient(${p.id})">Delete</button>
      </td>
    `;
    tbody.appendChild(tr);
  });
}

function filterPatients() {
  const query = document.getElementById("patientSearchInput").value.toLowerCase().trim();
  const filtered = patients.filter(p => 
    p.id.toString().includes(query) ||
    p.name.toLowerCase().includes(query) ||
    p.disease.toLowerCase().includes(query)
  );
  renderPatientsTable(filtered);
}

function handlePatientSubmit(e) {
  e.preventDefault();
  const name = document.getElementById("pName").value.trim();
  const age = parseInt(document.getElementById("pAge").value);
  const gender = document.getElementById("pGender").value;
  const phone = document.getElementById("pPhone").value.trim();
  const disease = document.getElementById("pDisease").value.trim();

  const payload = { name, age, gender, phone, disease };

  if (isHostedByC) {
    fetch("/api/patients", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payload)
    })
      .then(res => res.json())
      .then(data => {
        patients = data.patients || [];
        closeModal("modalAddPatient");
        document.getElementById("formAddPatient").reset();
        renderAll();
        alert(`Patient ${name} registered successfully into C array!`);
      });
  } else {
    const nextId = patients.length > 0 ? Math.max(...patients.map(p => p.id)) + 1 : 101;
    patients.push({ id: nextId, ...payload });
    localStorage.setItem("truecare_patients", JSON.stringify(patients));
    closeModal("modalAddPatient");
    document.getElementById("formAddPatient").reset();
    renderAll();
  }
}

function deletePatient(id) {
  const p = patients.find(item => item.id === id);
  if (!p) return;
  if (confirm(`Are you sure you want to delete patient '${p.name}' (ID: ${p.id}) from C storage?`)) {
    if (isHostedByC) {
      fetch("/api/patients/delete", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ id: id })
      })
        .then(res => res.json())
        .then(data => {
          patients = data.patients || [];
          renderAll();
        });
    } else {
      patients = patients.filter(item => item.id !== id);
      localStorage.setItem("truecare_patients", JSON.stringify(patients));
      renderAll();
    }
  }
}

// ============================================================================
// 3. DOCTOR CRUD
// ============================================================================
function renderDoctorsTable(list = doctors) {
  const tbody = document.getElementById("doctorTableBody");
  tbody.innerHTML = "";

  if (list.length === 0) {
    tbody.innerHTML = `<tr><td colspan="5" style="text-align:center; padding: 25px; color: var(--text-muted);">No doctors found.</td></tr>`;
    return;
  }

  list.forEach(d => {
    const tr = document.createElement("tr");
    tr.innerHTML = `
      <td><span class="badge badge-id">#${d.id}</span></td>
      <td><strong>Dr. ${d.name}</strong></td>
      <td><span class="badge badge-specialty">${d.specialization}</span></td>
      <td>${d.phone}</td>
      <td><span class="badge badge-active">Available</span></td>
    `;
    tbody.appendChild(tr);
  });
}

function filterDoctors() {
  const query = document.getElementById("doctorSearchInput").value.toLowerCase().trim();
  const filtered = doctors.filter(d => 
    d.name.toLowerCase().includes(query) ||
    d.specialization.toLowerCase().includes(query) ||
    d.id.toString().includes(query)
  );
  renderDoctorsTable(filtered);
}

function handleDoctorSubmit(e) {
  e.preventDefault();
  const name = document.getElementById("docName").value.replace(/^Dr\.\s*/i, "").trim();
  const specialization = document.getElementById("docSpecialization").value.trim();
  const phone = document.getElementById("docPhone").value.trim();

  const payload = { name, specialization, phone };

  if (isHostedByC) {
    fetch("/api/doctors", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payload)
    })
      .then(res => res.json())
      .then(data => {
        doctors = data.doctors || [];
        closeModal("modalAddDoctor");
        document.getElementById("formAddDoctor").reset();
        renderAll();
        alert(`Dr. ${name} added successfully into C doctor array!`);
      });
  } else {
    const nextId = doctors.length > 0 ? Math.max(...doctors.map(d => d.id)) + 1 : 201;
    doctors.push({ id: nextId, ...payload });
    localStorage.setItem("truecare_doctors", JSON.stringify(doctors));
    closeModal("modalAddDoctor");
    document.getElementById("formAddDoctor").reset();
    renderAll();
  }
}

// ============================================================================
// 4. APPOINTMENT CRUD
// ============================================================================
function renderAppointmentsTable(list = appointments) {
  const tbody = document.getElementById("appointmentTableBody");
  tbody.innerHTML = "";

  if (list.length === 0) {
    tbody.innerHTML = `<tr><td colspan="6" style="text-align:center; padding: 25px; color: var(--text-muted);">No appointments booked.</td></tr>`;
    return;
  }

  list.forEach(a => {
    const tr = document.createElement("tr");
    tr.innerHTML = `
      <td><span class="badge badge-id">#${a.id}</span></td>
      <td><strong>${a.patientName}</strong> <span style="color:var(--text-muted); font-size:12px;">(#${a.patientId})</span></td>
      <td>Dr. ${a.doctorName}</td>
      <td><span class="badge badge-date">${a.date}</span></td>
      <td><span class="badge badge-active">Confirmed</span></td>
      <td class="text-right">
        <button class="btn btn-danger-outline" onclick="cancelAppointment(${a.id})">Cancel</button>
      </td>
    `;
    tbody.appendChild(tr);
  });
}

function filterAppointments() {
  const query = document.getElementById("apptSearchInput").value.toLowerCase().trim();
  const filtered = appointments.filter(a => 
    a.patientName.toLowerCase().includes(query) ||
    a.doctorName.toLowerCase().includes(query) ||
    a.id.toString().includes(query)
  );
  renderAppointmentsTable(filtered);
}

function cancelAppointment(id) {
  if (confirm(`Cancel appointment #${id}?`)) {
    if (isHostedByC) {
      fetch("/api/appointments/cancel", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ id: id })
      })
        .then(res => res.json())
        .then(data => {
          appointments = data.appointments || [];
          renderAll();
        });
    } else {
      appointments = appointments.filter(a => a.id !== id);
      localStorage.setItem("truecare_appointments", JSON.stringify(appointments));
      renderAll();
    }
  }
}

function handleAppointmentSubmit(e) {
  e.preventDefault();
  const patientId = parseInt(document.getElementById("apptPatientSelect").value);
  const doctorId = parseInt(document.getElementById("apptDoctorSelect").value);
  const date = document.getElementById("apptDate").value;

  const payload = { patientId, doctorId, date };

  if (isHostedByC) {
    fetch("/api/appointments", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payload)
    })
      .then(res => res.json())
      .then(data => {
        appointments = data.appointments || [];
        closeModal("modalBookAppt");
        document.getElementById("formBookAppt").reset();
        renderAll();
        alert(`Appointment confirmed in C backend!`);
      });
  } else {
    const nextId = appointments.length > 0 ? Math.max(...appointments.map(a => a.id)) + 1 : 301;
    const p = patients.find(i => i.id === patientId);
    const d = doctors.find(i => i.id === doctorId);
    appointments.push({
      id: nextId,
      patientId,
      patientName: p ? p.name : "Unknown",
      doctorId,
      doctorName: d ? d.name : "Unknown",
      date
    });
    localStorage.setItem("truecare_appointments", JSON.stringify(appointments));
    closeModal("modalBookAppt");
    document.getElementById("formBookAppt").reset();
    renderAll();
  }
}

// ============================================================================
// 5. BILLING CRUD
// ============================================================================
function renderBillingTable(list = bills) {
  const tbody = document.getElementById("billTableBody");
  tbody.innerHTML = "";

  if (list.length === 0) {
    tbody.innerHTML = `<tr><td colspan="7" style="text-align:center; padding: 25px; color: var(--text-muted);">No invoices recorded.</td></tr>`;
    return;
  }

  list.forEach(b => {
    const tr = document.createElement("tr");
    tr.innerHTML = `
      <td><span class="badge badge-id">INV-${b.id}</span></td>
      <td><strong>${b.patientName}</strong></td>
      <td>Rs. ${parseFloat(b.consultation).toFixed(2)}</td>
      <td>Rs. ${parseFloat(b.medicine).toFixed(2)}</td>
      <td>Rs. ${parseFloat(b.treatment).toFixed(2)}</td>
      <td><strong style="color:var(--primary-blue-dark);">Rs. ${parseFloat(b.total).toFixed(2)}</strong></td>
      <td class="text-right">
        <button class="btn btn-outline btn-sm" onclick="openReceiptModal(${b.id})">View Receipt</button>
      </td>
    `;
    tbody.appendChild(tr);
  });
}

function filterBills() {
  const query = document.getElementById("billSearchInput").value.toLowerCase().trim();
  const filtered = bills.filter(b => 
    b.id.toString().includes(query) ||
    b.patientName.toLowerCase().includes(query)
  );
  renderBillingTable(filtered);
}

function calcBillPreview() {
  const consultation = parseFloat(document.getElementById("billConsultation").value) || 0;
  const medicine = parseFloat(document.getElementById("billMedicines").value) || 0;
  const treatment = parseFloat(document.getElementById("billTreatment").value) || 0;
  const total = consultation + medicine + treatment;

  document.getElementById("billPreviewTotal").textContent = `Rs. ${total.toLocaleString('en-IN', { minimumFractionDigits: 2 })}`;
}

function handleBillSubmit(e) {
  e.preventDefault();
  const patientId = parseInt(document.getElementById("billPatientSelect").value);
  const consultation = parseFloat(document.getElementById("billConsultation").value) || 0;
  const medicine = parseFloat(document.getElementById("billMedicines").value) || 0;
  const treatment = parseFloat(document.getElementById("billTreatment").value) || 0;
  const todayStr = new Date().toISOString().split("T")[0];

  const payload = { patientId, consultation, medicine, treatment, date: todayStr };

  if (isHostedByC) {
    fetch("/api/bills", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payload)
    })
      .then(res => res.json())
      .then(data => {
        bills = data.bills || [];
        closeModal("modalCreateBill");
        renderAll();
        const latest = bills[bills.length - 1];
        if (latest) openReceiptModal(latest.id);
      });
  } else {
    const nextId = bills.length > 0 ? Math.max(...bills.map(b => b.id)) + 1 : 401;
    const p = patients.find(i => i.id === patientId);
    const newBill = {
      id: nextId,
      patientId,
      patientName: p ? p.name : "Unknown",
      consultation,
      medicine,
      treatment,
      total: consultation + medicine + treatment,
      date: todayStr
    };
    bills.push(newBill);
    localStorage.setItem("truecare_bills", JSON.stringify(bills));
    closeModal("modalCreateBill");
    renderAll();
    openReceiptModal(newBill.id);
  }
}

function openReceiptModal(billId) {
  const bill = bills.find(b => b.id === billId);
  if (!bill) return;

  document.getElementById("recInvoiceId").textContent = `INV-${bill.id}`;
  document.getElementById("recDate").textContent = bill.date || new Date().toISOString().split("T")[0];
  document.getElementById("recPatientId").textContent = `#${bill.patientId}`;
  document.getElementById("recPatientName").textContent = bill.patientName;

  document.getElementById("recConsultation").textContent = `Rs. ${parseFloat(bill.consultation).toFixed(2)}`;
  document.getElementById("recMedicine").textContent = `Rs. ${parseFloat(bill.medicine).toFixed(2)}`;
  document.getElementById("recTreatment").textContent = `Rs. ${parseFloat(bill.treatment).toFixed(2)}`;
  document.getElementById("recTotal").textContent = `Rs. ${parseFloat(bill.total).toFixed(2)}`;

  openModal("modalInvoiceReceipt");
}

function resetToSampleData() {
  if (confirm("Reset all records to initial sample data in C storage?")) {
    if (isHostedByC) {
      fetch("/api/reset", { method: "POST" })
        .then(res => res.json())
        .then(data => {
          patients = data.patients || [];
          doctors = data.doctors || [];
          appointments = data.appointments || [];
          bills = data.bills || [];
          renderAll();
          alert("Records have been reset in the C engine.");
        });
    } else {
      localStorage.clear();
      fallbackLocalStorage();
      alert("Records reset.");
    }
  }
}

// ============================================================================
// MODAL HELPERS
// ============================================================================
function openModal(modalId) {
  const modal = document.getElementById(modalId);
  if (!modal) return;

  if (modalId === "modalAddPatient") {
    const nextId = patients.length > 0 ? Math.max(...patients.map(p => p.id)) + 1 : 101;
    document.getElementById("pIdPreview").value = `#${nextId}`;
  }

  if (modalId === "modalAddDoctor") {
    const nextId = doctors.length > 0 ? Math.max(...doctors.map(d => d.id)) + 1 : 201;
    document.getElementById("docIdPreview").value = `#${nextId}`;
  }

  if (modalId === "modalBookAppt") {
    const pSelect = document.getElementById("apptPatientSelect");
    const dSelect = document.getElementById("apptDoctorSelect");
    pSelect.innerHTML = "";
    patients.forEach(p => {
      const opt = document.createElement("option");
      opt.value = p.id;
      opt.textContent = `${p.name} (#${p.id} - ${p.disease})`;
      pSelect.appendChild(opt);
    });

    dSelect.innerHTML = "";
    doctors.forEach(d => {
      const opt = document.createElement("option");
      opt.value = d.id;
      opt.textContent = `Dr. ${d.name} (${d.specialization})`;
      dSelect.appendChild(opt);
    });

    const tomorrow = new Date();
    tomorrow.setDate(tomorrow.getDate() + 1);
    document.getElementById("apptDate").value = tomorrow.toISOString().split("T")[0];
  }

  if (modalId === "modalCreateBill") {
    const pSelect = document.getElementById("billPatientSelect");
    pSelect.innerHTML = "";
    patients.forEach(p => {
      const opt = document.createElement("option");
      opt.value = p.id;
      opt.textContent = `${p.name} (#${p.id} - ${p.disease})`;
      pSelect.appendChild(opt);
    });
    calcBillPreview();
  }

  modal.classList.add("active");
}

function closeModal(modalId) {
  const modal = document.getElementById(modalId);
  if (modal) modal.classList.remove("active");
}

document.querySelectorAll(".modal-overlay").forEach(overlay => {
  overlay.addEventListener("click", (e) => {
    if (e.target === overlay) closeModal(overlay.id);
  });
});
