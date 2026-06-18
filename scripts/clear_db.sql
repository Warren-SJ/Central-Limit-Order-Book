-- Clear all data from the tables while keeping the schema intact.
-- Deleted in order of foreign key dependencies.

DELETE FROM public.trades;
DELETE FROM public.orders;
DELETE FROM public.clients;
DELETE FROM public.tickers;
