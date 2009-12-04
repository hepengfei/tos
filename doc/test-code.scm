
list->vecTor

( (a . b) . (c . d))


(define elementwise
  (lambda (f)
    (lambda vectors
      (generate-vector
       (vector-length (car vectors))
       (lambda (i)
	 (apply f
		(map (lambda (v) (vector-ref v i))
		     vectors)))))))

(#\a #\newline #\b)


(a . b)


#\space



(define generate-vector
  (lambda (size proc)
    (let ((ans (make-vector size)))
      (letrec ((loop
		(lambda (i)
		  (cond ((= i size) ans)
			(else
			 (vector-set! ans i (proc i))
			 (loop (+ i 1)))))))
	(loop 0)))))


(define add-vectors (elementwise +))

(define scale-vector
  (lambda (s)
    (elementwise (lambda (x) (* x s)))))


(define map-streams
  (lambda (f s)
    (cons (f (head s))
	  (delay (map-streams f (tail s))))))



(define head car)
(define tail
  (lambda (stream) (force (cdr stream))))



(let ((n 50))
  (show-states n the-states))


;; 符号测试
(+
 a+
 *1/2
 /2
 =>
 >=?
 ASymbol
 lambda
 q
 list->veCTOR
 soup
 *2
 *
 V12a
 a34kTMNs
 the-word-resursion-has-many-meanings
 a.b
 !
 !a
 a!$%&+-./:<=>?@^_~)

;; 字符串测试
(""
 "common string"
 "special:\\ \" ' \' \( `,.~@#()[]{}_+|-=;"
 ;; 看包含换行的结果是怎么样的
 "newline
and something"
 ;; 结果是也包含一个换行符
 "tab	and sth")

;; 向量测试
#(#()
  #(a)
  #(a b)
  #( 3 2 1 (b c a) () )
  #( #() #(a) #((a b))))

;; 点对测试
((a)
 (a . b)
 (a . (b . c))
 (a . (b . ( c . () )))
 (a b c d e))

;;;;;;;;;;;;;;

(a (ba bb bc bd . (be . ())) c d . e)


()

(set! a 'b)

(#\a #\B #\b #\  #\space #\newline #\
 #\Z);space & newline

#()
#(1 a b c 2 '('list kebing tracy))

("" "hello" ".\vcf?'\'\"~`!@#$%^&*()_+|\\=-,./?><")

;this is comment

(eq? #f #t)

(1 2 +4 -434)

<=?





(define damped-oscillator
  (lambda (R L C)
    (lambda (state)
      (let ((Vc (vector-ref state 0))
	    (I1 (vector-ref state 1)))
	(vector (- 0 (+ (/ Vc (* R C)) (/ I1 C)))
		(/ Vc L))))))


(define the-states
  (integrate-system
   (damped-oscillator 10000 1000 .001)
   '#(1 0)
   .01))



(define show-states
  (lambda (n states)
    (if (> n 0)
	(begin
	  (display (vector->list (head states)))
	  (newline)
	  (show-states (- n 1) (tail states))))))
