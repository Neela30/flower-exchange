const dropZone = document.getElementById('dropZone');
const fileInput = document.getElementById('fileInput');
const fileInfo = document.getElementById('fileInfo');
const fileNameLabel = document.getElementById('fileName');
const browseBtn = document.getElementById('browseBtn');
const downloadBtn = document.getElementById('downloadBtn');
const statusBanner = document.getElementById('statusBanner');
const tableBody = document.getElementById('tableBody');
const pills = document.querySelectorAll('.pill');
const uploadHint = document.getElementById('uploadHint');

const counters = {
  New: document.getElementById('countNew'),
  Fill: document.getElementById('countFill'),
  PFill: document.getElementById('countPfill'),
  Rejected: document.getElementById('countReject'),
};

const flowerColors = {
  Rose: 'rose',
  Lavender: 'lavender',
  Lotus: 'lotus',
  Tulip: 'tulip',
  Orchid: 'orchid',
};

const statusClassMap = {
  New: 'status-new',
  Fill: 'status-fill',
  PFill: 'status-pfill',
  Rejected: 'status-reject',
};

let allData = [];
let currentFilter = 'ALL';
let csvCache = '';

function setStatus(message, variant) {
  if (!message) {
    statusBanner.classList.add('hidden');
    statusBanner.textContent = '';
    statusBanner.classList.remove('success', 'error');
    return;
  }
  statusBanner.textContent = message;
  statusBanner.classList.remove('hidden', 'success', 'error');
  statusBanner.classList.add(variant === 'error' ? 'error' : 'success');
}

function setLoading(isLoading) {
  dropZone.classList.toggle('dragover', isLoading);
  uploadHint.textContent = isLoading ? 'Processing orders on the engine...' : 'Upload a CSV to process it on the engine.';
}

function normalizeStatus(value) {
  if (!value) {
    return 'New';
  }
  const token = value.toString().toLowerCase();
  if (token === 'pfill') {
    return 'PFill';
  }
  if (token === 'fill') {
    return 'Fill';
  }
  if (token === 'rejected' || token === 'reject') {
    return 'Rejected';
  }
  if (token === 'new') {
    return 'New';
  }
  return value;
}

function normalizeSide(value) {
  const token = (value || '').toString().toLowerCase();
  if (token === 'sell') {
    return 'Sell';
  }
  return 'Buy';
}

function normalizeReports(reports) {
  return reports.map((report) => ({
    orderId: report.orderId || '-',
    clientOrderId: report.clientOrderId || '-',
    instrument: report.instrument || '',
    side: normalizeSide(report.side),
    status: normalizeStatus(report.status),
    quantity: Number(report.quantity ?? 0),
    price: Number(report.price ?? 0),
    reason: report.reason || '',
    timestamp: report.timestamp || '',
  }));
}

function updateStats(data) {
  const buckets = { New: 0, Fill: 0, PFill: 0, Rejected: 0 };
  data.forEach((row) => {
    buckets[row.status] = (buckets[row.status] || 0) + 1;
  });
  Object.entries(buckets).forEach(([key, value]) => {
    if (counters[key]) {
      counters[key].textContent = value.toString();
    }
  });
}

function renderEmptyState(message) {
  tableBody.innerHTML = `
    <tr>
      <td colspan="9">
        <div class="empty-state">
          <div class="icon" aria-hidden="true">&#127803;</div>
          <p>${message}</p>
        </div>
      </td>
    </tr>`;
}

function renderTable(data) {
  if (!data.length) {
    const message = currentFilter === 'ALL' && allData.length === 0
      ? 'Load an orders.csv to see execution reports'
      : 'No records found.';
    renderEmptyState(message);
    return;
  }

  const rows = data
    .map((row, index) => {
      const statusClass = statusClassMap[row.status] || statusClassMap.New;
      const sideClass = row.side === 'Buy' ? 'side-buy' : 'side-sell';
      const sideLabel = row.side === 'Buy' ? 'BUY' : 'SELL';
      const flowerClass = flowerColors[row.instrument] || 'rose';
      const delay = Math.min(index * 30, 300);
      const priceValue = Number.isFinite(row.price) ? row.price.toFixed(2) : '-';
      const quantityValue = Number.isFinite(row.quantity) ? row.quantity : '-';
      const reason = row.reason || '';
      const timestamp = row.timestamp || '';
      const instrument = row.instrument || '<span style="color:var(--muted)">-</span>';

      return `
        <tr style="animation-delay:${delay}ms">
          <td class="order-id">${row.orderId}</td>
          <td class="order-id">${row.clientOrderId}</td>
          <td>
            <div class="instrument">
              <span class="flower-dot ${flowerClass}"></span>
              ${instrument}
            </div>
          </td>
          <td><span class="side-badge ${sideClass}">${sideLabel}</span></td>
          <td><span class="status-badge ${statusClass}">${row.status}</span></td>
          <td class="price">${quantityValue}</td>
          <td class="price">${priceValue}</td>
          <td class="reason">${reason}</td>
          <td class="timestamp">${timestamp}</td>
        </tr>`;
    })
    .join('');

  tableBody.innerHTML = rows;
}

function filterTable(filterKey) {
  currentFilter = filterKey;
  pills.forEach((pill) => {
    pill.classList.toggle('active', pill.dataset.filter === filterKey);
  });

  const filtered =
    filterKey === 'ALL' ? allData : allData.filter((row) => row.status === filterKey);
  renderTable(filtered);
}

function buildCsvFromReports(data) {
  const header = 'order_id,client_order_id,instrument,side,status,executed_quantity,executed_price,reason,transaction_time';
  const lines = data.map(
    (row) =>
      `${row.orderId},${row.clientOrderId},${row.instrument},${row.side},${row.status},${row.quantity},${row.price},${row.reason},${row.timestamp}`
  );
  return [header, ...lines].join('\n');
}

function enableDownload(csvContent) {
  csvCache = csvContent;
  downloadBtn.disabled = !csvCache;
}

function triggerDownload() {
  if (!csvCache) {
    return;
  }

  const blob = new Blob([csvCache], { type: 'text/csv;charset=utf-8;' });
  const url = URL.createObjectURL(blob);
  const link = document.createElement('a');
  link.href = url;
  link.download = 'execution_report.csv';
  document.body.appendChild(link);
  link.click();
  document.body.removeChild(link);
  URL.revokeObjectURL(url);
}

async function uploadFile(file) {
  setLoading(true);
  setStatus('', 'success');
  const formData = new FormData();
  formData.append('orders', file);

  try {
    const response = await fetch('/api/execute', {
      method: 'POST',
      body: formData,
    });

    const payload = await response.json();
    if (!response.ok) {
      throw new Error(payload.error || 'Failed to process CSV.');
    }

    allData = normalizeReports(payload.reports || []);
    updateStats(allData);
    enableDownload(payload.csv || '');
    setStatus('Execution report ready.', 'success');
    filterTable(currentFilter);
  } catch (error) {
    allData = [];
    updateStats(allData);
    enableDownload('');
    setStatus(error.message || 'Unable to process file.', 'error');
    filterTable('ALL');
  } finally {
    setLoading(false);
  }
}

function handleFileSelection(event) {
  const file = event.target.files[0];
  if (!file) {
    return;
  }
  fileNameLabel.textContent = file.name;
  fileInfo.classList.remove('hidden');
  uploadFile(file);
  fileInput.value = '';
}

function handleDrop(event) {
  event.preventDefault();
  dropZone.classList.remove('dragover');
  const file = event.dataTransfer.files[0];
  if (file) {
    fileNameLabel.textContent = file.name;
    fileInfo.classList.remove('hidden');
    uploadFile(file);
  }
}

function initDragAndDrop() {
  dropZone.addEventListener('click', () => {
    fileInput.click();
  });

  dropZone.addEventListener('keydown', (event) => {
    if (event.key === 'Enter' || event.key === ' ') {
      event.preventDefault();
      fileInput.click();
    }
  });

  dropZone.addEventListener('dragover', (event) => {
    event.preventDefault();
    dropZone.classList.add('dragover');
  });

  dropZone.addEventListener('dragleave', () => dropZone.classList.remove('dragover'));
  dropZone.addEventListener('drop', handleDrop);
}

function initFilters() {
  pills.forEach((pill) => {
    pill.addEventListener('click', () => {
      filterTable(pill.dataset.filter);
    });
  });
}

function init() {
  initDragAndDrop();
  initFilters();
  fileInput.addEventListener('change', handleFileSelection);

  browseBtn.addEventListener('click', () => {
    fileInput.click();
  });

  downloadBtn.addEventListener('click', () => {
    triggerDownload();
  });

  filterTable('ALL');
}

init();
