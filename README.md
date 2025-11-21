# Game Shop Simulator

A first-person micro-tycoon prototype inspired by *Supermarket Simulator*, where you run a small video game store.  
You receive shipments of boxed games, stock shelves, set prices, and serve customers at the checkout.  
Your pricing and service quality affect the shop’s reputation, which then drives how many customers visit the next day.

This project was built in about one week as part of the **Adzap UE5 Take-Home Test**.

---

## Controls

**Movement & Camera**
- `W / A / S / D` – Move
- `Mouse` – Look around

**Interaction**
- `E` – Interact
- `F` – End of day

---

## Core Gameplay Loop

1. **Receive stock**
   - Shipments arrive as boxes containing multiple copies of different games.
   - You open the delivery boxes and place the boxed games on the store shelves.

2. **Set prices**
   - Each shelf “slot” represents a specific game.
   - Interacting with a slot opens a **price input widget**, letting you set the selling price for that game.
   - Each game also has a **market price** and a **popularity** value (stored in a data table).

3. **Customers arrive**
   - Customers spawn throughout the in-game day based on your **store reputation** and daily settings.
   - Each customer:
     - Chooses a game to look for, with a **weighted random** biased toward more popular games.
     - Checks whether the game is **in stock and visible on the shelf**.

4. **Purchase decision**
   - If the game is available, the customer evaluates whether to buy it based on:
     - Market price
     - Store price
     - Game popularity
     - A personal **price perception threshold** (e.g. 4.75 still feels like “4”)
   - Using a probability function, the customer either:
     - Buys the game and goes to checkout, OR
     - Refuses to buy (e.g. “too expensive”, “no price”, etc.)

5. **Checkout & cash**
   - The player operates a **checkout/register with a card reader**.
   - When a customer is ready to pay:
     - The item is scanned/validated.
     - The store’s cash is increased by the selling price.

6. **Client report & end-of-day summary**
   - When a customer leaves, a **Client Report** is generated containing:
     - Did they buy? (`HasBought`)
     - If not, why? (TooExpensive / NotFound / NoCashier / NoPrice)
     - Market price & paid price
   - At the end of the in-game day:
     - All reports are aggregated.
     - A **reputation delta** is computed from average client experience.
     - A day summary widget shows:
       - Total customers
       - Successful purchases
       - Reasons for failed purchases
       - Reputation change (%)
       - Starting / ending cash
       - Expected customers for the next day

7. **Next day**
   - Store reputation is updated and clamped between 0 and 1.
   - The number of customers for the following day is computed by interpolating between a **minimum and maximum** client count based on reputation.
   - The loop repeats: stock → price → customers → results → reputation.

---

## Systems & Simulation Details

### Pricing & purchase probability

- Each game has:
  - `MarketPrice`
  - `StorePrice`
  - `Popularity` (0–1)
- A **perceived price** is computed (e.g. 4.75 can be perceived as 4.00 depending on a threshold).
- The **purchase probability** decreases as the store price goes above market price.
- Sensitivity to price is **inversely tied to popularity**:
  - Less popular games → customers are *more* sensitive to price.
  - Very popular games → customers are *slightly more tolerant* of higher prices.

### Client experience & reputation

- For each client, an **experience score** in [-1, +1] is computed based on:
  - Discount vs market price if they bought.
  - Overpricing penalties.
  - Failure reasons:
    - TooExpensive (penalized based on how overpriced it was)
    - NotFound
    - NoCashier
    - NoPrice
- End of day:
  - The average experience is computed.
  - A **DailyReputationFactor** scales this average into a `DeltaReputation` (e.g. ±15% max per day).
  - Store reputation is updated and drives **next day’s customer count**.

### Time of day

- The in-game day runs over a configurable **DayDuration** in seconds.
- A helper function maps the elapsed day time to a “shop clock” between, for example, **08:00 and 17:00**, formatted as `"HH:MM"` for UI display.

---

## Build & Target Hardware

- **Engine:** Unreal Engine 5.4 (Blueprint + C++)
- **Platform:** Windows 10 / 11 (64-bit)
- **Build:** Packaged Windows build (`Shipping` or `Development`) – runs outside the editor
- **Recommended:**
  - Quad-core CPU
  - 8 GB RAM minimum (16 GB recommended)
  - Mid-range GPU with DX12 support

To run:

1. Extract the provided `.zip`.
2. Launch `GameShopSimulator.exe`.

---

## AI-Assisted Development

AI tools were used intentionally for parts of this project, as required by the test.

**Tools used:**
- ChatGPT (GPT-5.1 Thinking)

**What AI helped with:**

- **Game design & economy system**
  - Brainstorming and formalizing the **customer behavior**, **pricing logic**, **purchase probability**, and **reputation system**.
  - Designing how **popularity**, **market price**, and **store price** interact.

- **C++ BlueprintFunctionLibrary design**
  - Designing a C++ `BlueprintFunctionLibrary` (`UClientSimLibrary`) that:
    - Computes client experience per `FClientReport`.
    - Computes end-of-day reputation delta and average satisfaction.
    - Computes next day’s expected customer count.
    - Formats day summary data for UI display.
    - Computes perceived price and in-game time-of-day strings.

- **UI text & feedback**
  - Generating natural-sounding English lines for NPCs when leaving the shop based on:
    - Bought / didn’t buy
    - Reason (too expensive, not found, no cashier, no price)
  - Helping name variables and UI labels (e.g. “OpeningCash”, “Expected Customers Tomorrow”, etc.).

- **Iteration & debugging**
  - Discussing balance issues (e.g. when prices are too far above market, or 9.99 vs 10.00 behavior).
  - Adjusting equations so that:
    - Selling below market price increases reputation more.
    - Overpricing too much reliably causes bad feedback.

**Example prompt themes (summary):**
- “Design a purchase probability function based on market price, store price, popularity, and a perception threshold.”
- “Create a reputation system that aggregates client reports with different fail reasons.”
- “Help me move my Blueprint pricing logic into a C++ BlueprintFunctionLibrary exposed to Blueprints.”

---

## Assets & Credits

- Game Cases: nawaxman - https://3dexport.com/3d-model-ps4-game-case-242337?srsltid=AfmBOooaVcFg7GODpPLwWsAZlroqzlkNC-jiFKS7vhoezyvfJmByrZ3E
- Shelf: Marc Mons - https://www.turbosquid.com/3d-models/3d-model-black-book-shelf-v2-2423628?dd_referrer=
- Credit Card: Kerols65 - https://www.turbosquid.com/3d-models/bank-debit-card-1601393
- Mouse, Screen, Computer Table: Youssef Hamdan - https://www.fab.com/listings/37652522-c45e-486c-8da0-ba01ed632a05
- Delivery Box: Gvozdy - https://www.fab.com/listings/476e415a-e4f3-4b0d-a185-d1737d67b58c
- Cash register, keyboard, checkout table, cash register: Aleksandr Zhdanov - https://www.fab.com/fr/listings/2d8b4345-d813-4d0b-85fd-ca8f48cfa77f
- Characters, Characters Animations: Mixamo

## Known Issues

- When too many customers (AI) queue at the checkout, some of them may get stuck due to navigation congestion. In certain cases, clients block each other and are unable to exit the store properly.
- if the end of the day is triggered while the player is actively interacting with an object (holding or operating it), the player character may remain locked and unable to move until the next game cycle.

---

## What I Would Improve With More Time

### Code Quality & Architecture
- Refactor the systems into cleaner and more modular components (customer AI, pricing logic, reputation system, economy).
- Improve the separation between C++ and Blueprint logic to enhance readability and maintainability.
- Reduce unnecessary ticking, tracing, and overlapping checks for better performance.

### Performance & Optimization
- Improve AI pathfinding to avoid congestion issues around checkout and narrow areas.
- Optimize item and customer interactions for smoother performance during peak store traffic.
- Introduce customer pooling to avoid runtime spawning overhead.

### Visuals, Assets & Level Design
- Replace placeholder or basic assets with higher-quality models.
- Redesign the store layout for better flow, readability, and gameplay clarity.
- Enhance lighting, post-processing, and environment polish.
- Upgrade the UI with improved visuals, icons, transitions, and animations.

### Gameplay Features
- Add employee systems (e.g., hire a cashier or stocker).
- Implement self-checkout machines to reduce queue congestion.
- Expand the product catalog: consoles, accessories, merchandise, figurines, retro games, used games, etc.
- Add store expansion and upgrade mechanics (bigger shop, more shelves, faster checkout machines).
- Introduce customer personality types that influence buying behavior.
- Save/Load System

### AI & Animation
- Improve customer animations and add more idle, browsing, and reaction behaviors.
- Add more varied NPC meshes and outfits to avoid visual repetition.
- Enhance navigation and decision-making logic to prevent “deadlocks” in customer lines.

### Economy & Simulation Depth
- Refine purchase probability algorithms using additional data (rarity, trends, special events).
- Add dynamic market price fluctuations.
- Implement supplier systems, wholesale costs, and negotiation mechanics.
- Add more detailed financial reports (daily/weekly/monthly summaries).
  
---

## How this project meets the Adzap UE5 test criteria

- **Unreal Engine 5:**  
  Project built with UE 5.4 (C++ + Blueprints).

- **Playable Windows build:**  
  Packaged Windows build runs outside the editor.

- **First-person interaction:**
  - WASD + mouse movement/look.
  - Player can pick up, carry, and drop small physics objects (game cases, boxes).

- **Core loop with score/cash:**
  - Input: receive stock, place games, set prices.
  - Process: customers choose games, evaluate prices, decide to buy or not.
  - Output: cash and reputation are updated when purchases succeed.
  - End-of-day summary shows performance and affects next day’s clients.

- **Minimal UI:**
  - Cash display.
  - Price input widget.
  - Day summary widget (clients, reputation, cash, next day).

- **AI-assisted development:**
  - ChatGPT was used to help design and refine the systems (economy, reputation, C++ helpers, and text), as described above and demonstrated in the video.
