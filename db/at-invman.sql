
--

-- Table: employee
CREATE TABLE employee (
  Employee_ID INTEGER PRIMARY KEY,
  Employee_Name TEXT NOT NULL,
  Employee_Possition TEXT NOT NULL,
  Employee_Status TEXT NOT NULL,
  password_hash TEXT NOT NULL
);

-- Table: product
CREATE TABLE product (
  Product_Code TEXT PRIMARY KEY,
  Product_Name TEXT NOT NULL,
  Product_Qty INTEGER NOT NULL,
  Product_Price INTEGER NOT NULL
);

-- Table: purchase_orders
CREATE TABLE purchase_orders (
  PO_ID INTEGER PRIMARY KEY,
  CustomerName TEXT NOT NULL,
  OrderDate TEXT NOT NULL, -- DATE stored as TEXT (SQLite handles it this way)
  Status TEXT DEFAULT 'Pending'
);

-- Table: purchase_order_items
CREATE TABLE purchase_order_items (
  Item_ID INTEGER PRIMARY KEY,
  PO_ID INTEGER NOT NULL,
  Product_Code TEXT NOT NULL,
  QuantityOrdered INTEGER NOT NULL,
  PurchasePrice INTEGER NOT NULL,
  FOREIGN KEY (PO_ID) REFERENCES purchase_orders(PO_ID),
  FOREIGN KEY (Product_Code) REFERENCES product(Product_Code)
);

-- Table: schedules
CREATE TABLE schedules (
  schedule_id INTEGER PRIMARY KEY,
  Employee_ID INTEGER,
  shift_date TEXT NOT NULL,     -- stored as TEXT in YYYY-MM-DD
  start_time TEXT NOT NULL,     -- stored as TEXT in HH:MM:SS
  end_time TEXT NOT NULL,
  notes TEXT DEFAULT NULL,
  FOREIGN KEY (Employee_ID) REFERENCES employee(Employee_ID)
);

