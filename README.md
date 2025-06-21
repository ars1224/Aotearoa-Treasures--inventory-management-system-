# 🗃️ Aotearoa Treasures – Inventory Management System

Welcome to our group project for **SD103 Integrated Studio** at **Yoobee College**. This system is developed to help manage inventory, sales, and employee operations for Aotearoa Treasures Ltd.

---

## 📚 Project Overview

This inventory management system is designed to:
- Track and manage stock across multiple branches (Auckland, Christchurch, Wellington)
- Support point-of-sale operations
- Generate various reports (sales, stock movement, low stock)
- Manage employee data and schedules

---

## 🚀 Features

- **Inventory Management**  
  Add, delete, update, and track stock by region with quantity validation.

- **Point of Sale (POS)**  
  Allows product scanning, cart management, and receipt generation.

- **Employee Management**  
  Add, list, and remove employees; manage authentication roles.

- **Roster System**  
  Schedule and view employee rosters.

- **Reports**  
  Generate weekly/monthly sales and stock reports.

- **Database Integration**  
  Powered by SQLite for efficient local data handling.

---

## 🛠️ Tech Stack

- C++
- SQLite
- Visual Studio 2022
- DB Browser for SQLite
- Git & GitHub

---

## 👥 Contributors & Scrum Roles

In **Group 5**, we adopted Scrum methodology and assigned ourselves the following roles:

| Name            | GitHub                                      | Scrum Role(s)                                   | Project Role / Feature Focus              |
|-----------------|---------------------------------------------|--------------------------------------------------|-------------------------------------------|
| Cameron Moss    | [@Camerondmoss98](https://github.com/Camerondmoss98) | Scrum Master, Developer          | Inventory Module, Team Coordination       |
| Dhona Obina     | [@dhonaobina](https://github.com/dhonaobina)         | Product Owner,Developer                                       | Point of Sale (POS) Module, Git & Repo Setup |
| Aries (ars1224) | [@ars1224](https://github.com/ars1224)               | Developer, Stakeholder Representative           | Database Integration, Authentication, Reports |

---

### 📌 Project Sponsor

**Ms. Maia Fox**  
Managing Director – *Aotearoa Treasures Ltd.*  
*(As given in Scenario 2)*

---

## 🧪 How to Run the Project

1. Clone the repository:
   ```bash
   git clone https://github.com/ars1224/Aotearoa-Treasures--inventory-management-system.git
2. Open the .sln file in Visual Studio.

3. Ensure SQLite3 is included and the db/at-invman.db file is accessible.

4. Build and run the solution:

- Use the Debug > x64 build configuration.

- Start the project from AT-InvMan.

---

## ✅ Version Control Best Practices

To keep the project stable and avoid merge conflicts, please follow these steps before pushing your code:

```bash
# 🔄 1. Pull the latest changes from the main branch
git pull origin main

# 📝 2. Stage and commit your changes
git add .
git commit -m "Your descriptive commit message"

# ⬆️ 3. Push your changes to the main branch
git push origin main
---


