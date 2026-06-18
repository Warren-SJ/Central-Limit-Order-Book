-- Seed initial tickers
INSERT INTO public.tickers (id, symbol) VALUES
(1, 'AAPL'),
(2, 'MSFT'),
(3, 'GOOGL'),
(4, 'AMZN'),
(5, 'TSLA'),
(6, 'FB'),
(7, 'NVDA'),
(8, 'JPM'),
(9, 'V'),
(10, 'DIS'),
(11, 'NFLX'),
(12, 'ADBE'),
(13, 'PYPL'),
(14, 'INTC'),
(15, 'CSCO')
ON CONFLICT (id) DO NOTHING;

-- Seed initial clients
INSERT INTO public.clients (id, name, contact, email, address) VALUES
(1, 'Alice Smith', '1234567890', 'alice@example.com', '123 Wall St'),
(2, 'Bob Jones', '0987654321', 'bob@example.com', '456 Broad St'),
(3, 'Charlie Brown', '5551234567', 'charlie@example.com', '789 Market St'),
(4, 'Diana Prince', '5559876543', 'diana@example.com', '321 Elm St'),
(5, 'Ethan Hunt', '5555555555', 'ethan@example.com', '654 Oak St'),
(6, 'Fiona Gallagher', '5551112222', 'fiona@xample.com', '987 Pine St'),
(7, 'George Martin', '5553334444', 'george@xample.com', '159 Maple St'),
(8, 'Hannah Lee', '5557778888', 'hanna@xample.com', '753 Cedar St'),
(9, 'Ian Wright', '5559990000', 'ian@xample.com', '852 Birch St'),
(10, 'Jane Doe', '5552223333', 'jane@xample.com', '456 Spruce St'),
(11, 'Kevin Hart', '5554445555', 'kevin@xample.com', '321 Willow St'),
(12, 'Laura Palmer', '5556667777', 'laura@xample.com', '654 Aspen St'),
(13, 'Michael Scott', '5558889999', 'michael@xample.com', '987 Redwood St'),
(14, 'Nina Dobrev', '5550001111', 'nina@xample.com', '159 Cypress St'),
(15, 'Oscar Wilde', '5551231234', 'oscar@xample.com', '753 Fir St')
ON CONFLICT (id) DO NOTHING;
