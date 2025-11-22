CREATE LLM MODEL gemini PATH 'gemini-2.5-flash' OUTPUT(...); ;
CREATE LLM MODEL gemma3 PATH 'gemma3:12b' OUTPUT(...); ;
CREATE LLM MODEL llama3 PATH 'llama3.2:latest' OUTPUT(...); ;
CREATE LLM MODEL o4mini PATH 'o4-mini' OUTPUT(...); ;
CREATE TABLE cast_info(id BIGINT, person_id BIGINT, movie_id BIGINT, person_role_id BIGINT, note VARCHAR, nr_order BIGINT, role_id BIGINT);;
CREATE TABLE info_type(id BIGINT, info VARCHAR);;
CREATE TABLE movie(id BIGINT, title VARCHAR, production_year BIGINT, kind VARCHAR, runtime_mins DOUBLE, genre VARCHAR, lang VARCHAR, director VARCHAR, writer VARCHAR);;
CREATE TABLE movie_info(id BIGINT, movie_id BIGINT, info_type_id BIGINT, info VARCHAR, note VARCHAR);;
CREATE TABLE movie_sample(id BIGINT, title VARCHAR, production_year BIGINT, kind VARCHAR, runtime_mins DOUBLE, genre VARCHAR, lang VARCHAR, director VARCHAR, writer VARCHAR);;
CREATE TABLE "name"(id BIGINT, "name" VARCHAR, imdb_index VARCHAR, imdb_id BIGINT, gender VARCHAR, name_pcode_cf VARCHAR, name_pcode_nf VARCHAR, surname_pcode VARCHAR, md5sum VARCHAR);;
CREATE TABLE review(id_tomato VARCHAR, review_id BIGINT, created_on VARCHAR, critic_name VARCHAR, is_top_critic BOOLEAN, score VARCHAR, state VARCHAR, "publication" VARCHAR, review VARCHAR, sentiment VARCHAR, url VARCHAR, movie_id BIGINT);;
CREATE TABLE role_type(id BIGINT, "role" VARCHAR);;

