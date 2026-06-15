BEGIN;


CREATE TABLE IF NOT EXISTS public.clients
(
    id bigint NOT NULL,
    name text COLLATE pg_catalog."default" NOT NULL,
    contact text COLLATE pg_catalog."default" NOT NULL,
    email text COLLATE pg_catalog."default" NOT NULL,
    address text COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT clients_pkey PRIMARY KEY (id)
    );

CREATE TABLE IF NOT EXISTS public.orders
(
    id bigint NOT NULL,
    client bigint NOT NULL,
    price integer NOT NULL,
    original_quantity integer NOT NULL,
    ticker bigint NOT NULL,
    remaining_quantity integer NOT NULL,
    status "char" NOT NULL,
    type "char" NOT NULL,
    "timestamp" timestamp with time zone NOT NULL,
    CONSTRAINT orders_pkey PRIMARY KEY (id)
    );

CREATE TABLE IF NOT EXISTS public.tickers
(
    id bigint NOT NULL,
    symbol text COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT tickers_pkey PRIMARY KEY (id)
    );

CREATE TABLE IF NOT EXISTS public.trades
(
    id bigint NOT NULL,
    buyer bigint NOT NULL,
    seller bigint NOT NULL,
    price integer NOT NULL,
    quantity integer NOT NULL,
    ticker bigint NOT NULL,
    "timestamp" timestamp with time zone NOT NULL,
    CONSTRAINT trades_pkey PRIMARY KEY (id)
    );

ALTER TABLE IF EXISTS public.orders
    ADD CONSTRAINT clientkey FOREIGN KEY (client)
    REFERENCES public.clients (id) MATCH SIMPLE
    ON UPDATE CASCADE
       ON DELETE RESTRICT
    NOT VALID;


ALTER TABLE IF EXISTS public.orders
    ADD CONSTRAINT tickerkey FOREIGN KEY (ticker)
    REFERENCES public.tickers (id) MATCH SIMPLE
    ON UPDATE CASCADE
       ON DELETE RESTRICT
    NOT VALID;


ALTER TABLE IF EXISTS public.trades
    ADD CONSTRAINT buyerconstraint FOREIGN KEY (buyer)
    REFERENCES public.clients (id) MATCH SIMPLE
    ON UPDATE CASCADE
       ON DELETE RESTRICT;


ALTER TABLE IF EXISTS public.trades
    ADD CONSTRAINT sellerconstraint FOREIGN KEY (seller)
    REFERENCES public.clients (id) MATCH SIMPLE
    ON UPDATE CASCADE
       ON DELETE RESTRICT;


ALTER TABLE IF EXISTS public.trades
    ADD CONSTRAINT tickerconstraint FOREIGN KEY (ticker)
    REFERENCES public.tickers (id) MATCH SIMPLE
    ON UPDATE CASCADE
       ON DELETE RESTRICT
    NOT VALID;

END;
