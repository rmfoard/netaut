select * into sdxcrpt from (
    (select * from sd where initnrnodes = 256 and iterationnr <> 0)
union
    (select * from sd where initnrnodes = 512 and iterationnr <> 0)
union
    (select * from sd where initnrnodes = 768 and iterationnr <> 0)
union
    (select * from sd where initnrnodes = 1024 and iterationnr <> 0)
);
