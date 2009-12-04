
;; 3.14

(define list? (list)
  (if (null? list)
      #t
      (if (pair? list)
	  (list? (cdr list))
	  #f)))

;;  (define (loop x y)
;;    (if (null? x)
;;	y
	(let ((temp (cdr x)))
	  (set-cdr! x y)
	  (loop temp x));))




(define (mystery x)
  (define (loop x y)
    (if (null? x)
	y
	(let ((temp (cdr x)))
	  (set-cdr! x y)
	  (loop temp x))))
  (loop x '()))

(define v (list 'a 'b 'c 'd))

(define w (mystery v))
