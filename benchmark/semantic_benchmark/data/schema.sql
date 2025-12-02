CREATE TABLE cast_info(id BIGINT, person_id BIGINT, movie_id BIGINT, person_role_id BIGINT, note VARCHAR, nr_order BIGINT, role_id BIGINT);
CREATE TABLE info_type(id BIGINT, info VARCHAR);
CREATE TABLE movie(id BIGINT, title VARCHAR, runtime_mins DOUBLE, genre VARCHAR, lang VARCHAR, director VARCHAR, writer VARCHAR);
CREATE TABLE movie_info(id BIGINT, movie_id BIGINT, info_type_id BIGINT, info VARCHAR, note VARCHAR);
CREATE TABLE "name"(id BIGINT, "name" VARCHAR, imdb_index VARCHAR, imdb_id BIGINT, gender VARCHAR, name_pcode_cf VARCHAR, name_pcode_nf VARCHAR, surname_pcode VARCHAR, md5sum VARCHAR);
CREATE TABLE review(id_tomato VARCHAR, review_id BIGINT, created_on VARCHAR, critic_name VARCHAR, is_top_critic BOOLEAN, score VARCHAR, state VARCHAR, "publication" VARCHAR, review VARCHAR, sentiment VARCHAR, url VARCHAR, movie_id BIGINT);
CREATE TABLE role_type(id BIGINT, "role" VARCHAR);

