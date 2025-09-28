-- look up secrets
SELECT * FROM duckdb_secrets();

-- look up models
SELECT * FROM ipdb_models();

-- create remote model
CREATE LLM MODEL o4mini PATH 'o4-mini' ON PROMPT API 'https://api.openai.com/v1/' SECRET openai_key OPTIONS {"req_per_min": 30};

-- create fanar model
CREATE LLM MODEL fanar PATH 'Fanar' ON PROMPT API 'https://api.fanar.qa/v1/' SECRET qcri OPTIONS {"req_per_min": 30};

-- create local model
CREATE LLM MODEL gemma2 PATH '/scratch1/ukumaras/models/llm/gemma-2-2b-it.gguf' ON PROMPT;

-- view models
SELECT model_name, model_type, model_path FROM ipdb_models();

SET llm_use_batch = false;
SET ml_batch_size = 16;
SET llm_use_cache = false;
SET pull_predict_filter = false;

-- Get the total value of orders sent to each state.
SELECT * FROM Orders LIMIT 5;
SELECT * FROM Customer LIMIT 5;

SELECT state, sum(total_amount)
FROM LLM o4mini (PROMPT 'extract the {state VARCHAR} from the {{address}}', Customer) AS c
JOIN Orders AS o
ON o.customer_id = c.customer_id
GROUP BY state;

SELECT sum(total_amount)
FROM Customer AS c
JOIN Orders AS o
ON o.customer_id = c.customer_id
GROUP BY LLM o4mini (PROMPT 'extract the {state VARCHAR} from the {{address}}');

-- Get all the states in the US and their sales taxes.
SELECT state, state_tax 
FROM LLM o4mini (PROMPT 'get all the {state VARCHAR} and their {state_tax DOUBLE} pairs and the in the US');

-- Is the value of every GPU in inventory has gone up or down in price. 
SELECT * FROM Product LIMIT 5;
SELECT * FROM Category LIMIT 5;

SELECT p.name, price, LLM o4mini (PROMPT 'has the {{price}} of {{p.name}} gone up or {gone_down BOOLEAN} from the release date dollar price')
FROM Product AS p
JOIN Category AS c ON p.category_id = c.category_id
WHERE c.name = 'GPU';

-- Get negative reviews about CPUs.
SELECT p.name, r.review_text
FROM Product AS p 
JOIN Category AS c ON p.category_id = c.category_id
JOIN Review AS r ON p.product_id = r.product_id
WHERE c.name = 'CPU';

SELECT p.name, r.review_text
FROM Product AS p 
JOIN Category AS c ON p.category_id = c.category_id
JOIN Review AS r ON p.product_id = r.product_id
WHERE c.name = 'CPU'
AND LLM o4mini (PROMPT 'is the {sentiment VARCHAR} of the {{review_text}} positive or negative') = 'negative';

SELECT p.name, r.review_text
FROM Product AS p 
JOIN Category AS c ON p.category_id = c.category_id
JOIN LLM o4mini (PROMPT 'is the {sentiment VARCHAR} of the {{review_text}} positive or negative', Review) AS r ON p.product_id = r.product_id
WHERE c.name = 'CPU'
AND sentiment = 'negative';

-- Get motherboards compatible with 'AMD Ryzen 9 7950X'
FROM Product LIMIT 5;
FROM Category LIMIT 5;

SELECT p.name
FROM Product AS p
JOIN Category AS c ON p.category_id = c.category_id
JOIN Product AS o ON LLM o4mini (PROMPT 'is CPU {{o.name}} {compatible BOOLEAN} with motherboard {{p.name}}')
WHERE c.name = 'Motherboard' AND o.name = 'AMD Ryzen 9 7950X';

SELECT CPUs.name, Motherboards.name
FROM (Product AS p JOIN Category AS c1 ON p.category_id = c1.category_id AND c1.name = 'Motherboard') AS Motherboards
JOIN (Product AS o JOIN Category AS c2 ON o.category_id = c2.category_id AND c2.name = 'CPU') AS CPUs
ON LLM o4mini (PROMPT 'is CPU {{CPUs.name}} {compatible BOOLEAN} with motherboard {{Motherboards.name}}');

SET ml_batch_size = 16;
CREATE TABLE review_results_2 AS SELECT *
FROM food_review
WHERE LLM o4mini(PROMPT 'is the {review_type VARCHAR} of {{review}} is about. Only choose `food` or `service`.') = 'food';


CREATE TABLE review_sample_results AS SELECT *
FROM food_review_sample
WHERE LLM o4mini(PROMPT 'is the {review_type VARCHAR} of {{review}} is about `food` or `service`.') = 'food';

CREATE TABLE review_sample_filter_fanar AS SELECT *
FROM food_review_sample
WHERE LLM fanar (PROMPT 'is this {{review}} is about {is_food BOOLEAN}');

SELECT r.review_id, r.user_id, b.name, r.text
FROM review r
JOIN business b ON r.business_id = b.business_id
WHERE POSITION('Italian' IN b.categories) > 0
  AND b.stars >= 4.0
  AND b.review_count > 100;

SELECT r.review_id, r.user_id, b.name, r.text
FROM review r
JOIN business b ON r.business_id = b.business_id
WHERE llm('extract the {sentiment VARCHAR} of the review', r.text) = 'negative'
  AND POSITION('Italian' IN b.categories) > 0
  AND b.stars >= 4.0
  AND b.review_count > 100;

SELECT *, reason
FROM LLM ("{reason VARCHAR} and {is_negative BOOLEAN} of the review", Reviews)
WHERE is_negative;

SELECT category_id, LLM AGG o4mini (PROMPT 'what is the computer {component VARCHAR} from the {{description}}') FROM Product GROUP BY category_id;