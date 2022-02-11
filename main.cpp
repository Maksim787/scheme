#include <cassert>
#include "scheme.h"

class SchemeTest {
public:
    void ExpectEq(std::string expression, const std::string& result) {
        assert(interpreter_.Run(expression) == result);
    }

    void Execute(std::string expression) {
        interpreter_.Run(expression);
    }

private:
    Interpreter interpreter_;
};


int main() {
    {
        // Bolean
        SchemeTest t;

        t.ExpectEq("#t", "#t");
        t.ExpectEq("#f", "#f");

        t.ExpectEq("(boolean? #t)", "#t");
        t.ExpectEq("(boolean? #f)", "#t");
        t.ExpectEq("(boolean? 1)", "#f");
        t.ExpectEq("(boolean? '())", "#f");

        t.ExpectEq("(not #f)", "#t");
        t.ExpectEq("(not #t)", "#f");
        t.ExpectEq("(not 1)", "#f");
        t.ExpectEq("(not 0)", "#f");
        t.ExpectEq("(not '())", "#f");

        // (and <test>)
        // The <test> expressions are evaluated from left to right, and the value of the first
        // expression that evaluates to a false value is returned. Any remaining expressions are not
        // evaluated. If all the expressions evaluate to true values, the value of the last expression
        // is returned. If there are no expressions then #t is returned.

        t.ExpectEq("(and)", "#t");
        t.ExpectEq("(and (= 2 2) (> 2 1))", "#t");
        t.ExpectEq("(and (= 2 2) (< 2 1))", "#f");
        t.ExpectEq("(and 1 2 'c '(f g))", "(f g)");

        // (or <test>)
        // The <test> expressions are evaluated from left to right, and the value of the first
        // expression that evaluates to a true value is returned. Any remaining expressions are not
        // evaluated. If all expressions evaluate to false values, the value of the last expression is
        // returned. If there are no expressions then #f is returned.

        t.ExpectEq("(or)", "#f");
        t.ExpectEq("(or (not (= 2 2)) (> 2 1))", "#t");
        t.ExpectEq("(or #f (< 2 1))", "#f");
        t.ExpectEq("(or #f 1)", "1");
    }

    {
        // Quote
        SchemeTest t;


        t.ExpectEq("(quote (1 2))", "(1 2)");
        t.ExpectEq("'(1 2)", "(1 2)");
    }

    {
        // Integer
        SchemeTest t;

        t.ExpectEq("4", "4");
        t.ExpectEq("-14", "-14");
        t.ExpectEq("+14", "14");

        t.ExpectEq("(number? -1)", "#t");
        t.ExpectEq("(number? 1)", "#t");
        t.ExpectEq("(number? #t)", "#f");

        // =, <, >, <=, >=
        // These procedures return #t if their arguments are (respectively): equal,
        // monotonically increasing, monotonically decreasing, monotonically nondecreasing,
        // or monotonically nonincreasing.

        t.ExpectEq("(=)", "#t");
        t.ExpectEq("(>)", "#t");
        t.ExpectEq("(<)", "#t");
        t.ExpectEq("(>=)", "#t");
        t.ExpectEq("(<=)", "#t");

        t.ExpectEq("(= 1 2)", "#f");
        t.ExpectEq("(= 1 1)", "#t");
        t.ExpectEq("(= 1 1 1)", "#t");
        t.ExpectEq("(= 1 1 2)", "#f");

        t.ExpectEq("(> 2 1)", "#t");
        t.ExpectEq("(> 1 1)", "#f");
        t.ExpectEq("(> 3 2 1)", "#t");
        t.ExpectEq("(> 3 2 3)", "#f");

        t.ExpectEq("(< 1 2)", "#t");
        t.ExpectEq("(< 1 1)", "#f");
        t.ExpectEq("(< 1 2 3)", "#t");
        t.ExpectEq("(< 1 2 1)", "#f");

        t.ExpectEq("(>= 2 1)", "#t");
        t.ExpectEq("(>= 1 2)", "#f");
        t.ExpectEq("(>= 3 3 2)", "#t");
        t.ExpectEq("(>= 3 3 4)", "#f");

        t.ExpectEq("(<= 2 1)", "#f");
        t.ExpectEq("(<= 1 2)", "#t");
        t.ExpectEq("(<= 3 3 4)", "#t");
        t.ExpectEq("(<= 3 3 2)", "#f");

        t.ExpectEq("(+ 1 2)", "3");
        t.ExpectEq("(+ 1)", "1");
        t.ExpectEq("(+ 1 (+ 3 4 5))", "13");
        t.ExpectEq("(- 1 2)", "-1");
        t.ExpectEq("(- 2 1)", "1");
        t.ExpectEq("(- 2 1 1)", "0");
        t.ExpectEq("(* 5 6)", "30");
        t.ExpectEq("(* 5 6 7)", "210");
        t.ExpectEq("(/ 4 2)", "2");
        t.ExpectEq("(/ 4 2 2)", "1");

        t.ExpectEq("(+)", "0");
        t.ExpectEq("(*)", "1");

        t.ExpectEq("(max 0)", "0");
        t.ExpectEq("(min 0)", "0");

        t.ExpectEq("(max 1 2)", "2");
        t.ExpectEq("(min 1 2)", "1");

        t.ExpectEq("(max 1 2 3 4 5)", "5");
        t.ExpectEq("(min 1 2 3 4 5)", "1");

        t.ExpectEq("(abs 10)", "10");
        t.ExpectEq("(abs -10)", "10");
    }

    {
        // List
        SchemeTest t;

        t.ExpectEq("'()", "()");
        t.ExpectEq("'(1)", "(1)");
        t.ExpectEq("'(1 2)", "(1 2)");

        t.ExpectEq("'(1 . 2)", "(1 . 2)");

        t.ExpectEq("'(1 2 . 3)", "(1 2 . 3)");
        t.ExpectEq("'(1 2 . ())", "(1 2)");
        t.ExpectEq("'(1 . (2 . ()))", "(1 2)");

        t.ExpectEq("(pair? '(1 . 2))", "#t");
        t.ExpectEq("(pair? '(1 2))", "#t");
        t.ExpectEq("(pair? '())", "#f");

        t.ExpectEq("(null? '())", "#t");
        t.ExpectEq("(null? '(1 2))", "#f");
        t.ExpectEq("(null? '(1 . 2))", "#f");

        t.ExpectEq("(list? '())", "#t");
        t.ExpectEq("(list? '(1 2))", "#t");
        t.ExpectEq("(list? '(1 . 2))", "#f");
        t.ExpectEq("(list? '(1 2 3 4 . 5))", "#f");

        t.ExpectEq("(cons 1 2)", "(1 . 2)");

        t.ExpectEq("(car '(1 . 2))", "1");
        t.ExpectEq("(car '(1))", "1");
        t.ExpectEq("(car '(1 2 3))", "1");

        t.ExpectEq("(cdr '(1 . 2))", "2");
        t.ExpectEq("(cdr '(1))", "()");
        t.ExpectEq("(cdr '(1 2))", "(2)");
        t.ExpectEq("(cdr '(1 2 3))", "(2 3)");
        t.ExpectEq("(cdr '(1 2 3 . 4))", "(2 3 . 4)");

        t.ExpectEq("(list)", "()");
        t.ExpectEq("(list 1)", "(1)");
        t.ExpectEq("(list 1 2 3)", "(1 2 3)");

        t.ExpectEq("(list-ref '(1 2 3) 1)", "2");
        t.ExpectEq("(list-tail '(1 2 3) 1)", "(2 3)");
        t.ExpectEq("(list-tail '(1 2 3) 3)", "()");
    }

    {
        // If
        SchemeTest t;

        t.ExpectEq("(if #t 0)", "0");
        t.ExpectEq("(if #f 0)", "()");
        t.ExpectEq("(if (= 2 2) (+ 1 10))", "11");
        t.ExpectEq("(if (= 2 3) (+ 1 10) 5)", "5");

        t.Execute("(define x 1)");

        t.Execute("(if #f (set! x 2))");
        t.ExpectEq("x", "1");

        t.Execute("(if #t (set! x 4) (set! x 3))");
        t.ExpectEq("x", "4");
    }

    {
        // SymbolsAreNotSelfEvaluating
        SchemeTest t;
        t.ExpectEq("'x", "x");
        t.ExpectEq("(quote x)", "x");
    }

    {
        // SymbolPredicate
        SchemeTest t;
        t.ExpectEq("(symbol? 'x)", "#t");
        t.ExpectEq("(symbol? 1)", "#f");
    }

    {
        // SymbolsAreUsedAsVariableNames
        SchemeTest t;
        t.Execute("(define x (+ 1 2))");
        t.ExpectEq("x", "3");

        t.Execute("(define x (+ 2 2))");
        t.ExpectEq("x", "4");
    }

    {
        // SetOverrideVariables
        SchemeTest t;

        t.Execute("(define x 1)");
        t.ExpectEq("x", "1");

        t.Execute("(set! x (+ 2 4))");
        t.ExpectEq("x", "6");
    }

    {
        // CopySemantics
        SchemeTest t;

        t.Execute("(define x 1)");
        t.Execute("(define y x)");
        t.ExpectEq("x", "1");
        t.ExpectEq("y", "1");
        t.Execute("(define x y)");
        t.Execute("(set! y (+ 1 y))");
        t.ExpectEq("y", "2");
        t.ExpectEq("x", "1");
    }

    {
        // PairMutations
        SchemeTest t;

        t.Execute("(define x '(1 . 2))");

        t.Execute("(set-car! x 5)");
        t.ExpectEq("(car x)", "5");

        t.Execute("(set-cdr! x 6)");
        t.ExpectEq("(cdr x)", "6");

        t.Execute("(define z 1543)");

        t.Execute("(set-cdr! x z)");
        t.ExpectEq("(cdr x)", "1543");

        t.Execute("(set-car! x z)");
        t.ExpectEq("(car x)", "1543");
    }

    {
        // SimpleLambda
        SchemeTest t;

        t.ExpectEq("((lambda (x) (+ 1 x)) 5)", "6");
    }

    {
        // LambdaBodyHasImplicitBegin
        SchemeTest t;

        t.Execute("(define test (lambda (x) (set! x (* x 2)) (+ 1 x)))");
        t.ExpectEq("(test 20)", "41");
    }

    {
        // SlowSum
        SchemeTest t;

        t.Execute("(define slow-add (lambda (x y) (if (= x 0) y (slow-add (- x 1) (+ y 1)))))");
        t.ExpectEq("(slow-add 3 3)", "6");
        t.ExpectEq("(slow-add 100 100)", "200");
    }

    {
        // LambdaClosure
        SchemeTest t;

        t.Execute("(define x 1)");

        t.Execute(R"EOF(
        (define range
          (lambda (x)
            (lambda ()
              (set! x (+ x 1))
              x)))
                    )EOF");

        // (define range (lambda (x) (lambda () (set! x (+ x 1)) x)))

        t.Execute("(define my-range (range 10))");
        t.ExpectEq("(my-range)", "11");
        t.ExpectEq("(my-range)", "12");
        t.ExpectEq("(my-range)", "13");

        t.ExpectEq("x", "1");
    }

    {
        // DefineLambdaSugar
        SchemeTest t;

        t.Execute("(define (inc x) (+ x 1))");
        t.ExpectEq("(inc -1)", "0");

        t.Execute("(define (add x y) (+ x y 1))");
        t.ExpectEq("(add -10 10)", "1");

        t.Execute("(define (zero) 0)");
        t.ExpectEq("(zero)", "0");
    }

    {
        // LambdaMultipleRecurseCalls
        SchemeTest t;

        t.Execute("(define (fib x) (if (< x 3) 1 (+ (fib (- x 1)) (fib (- x 2)) )))");
        t.ExpectEq("(fib 1)", "1");
        t.ExpectEq("(fib 2)", "1");
        t.ExpectEq("(fib 3)", "2");
        t.ExpectEq("(fib 7)", "13");
        t.ExpectEq("(fib 8)", "21");
    }

    {
        // MutualCalls
        SchemeTest t;

        t.Execute("(define (foo x) (if (< x 2) 42 (bar (- x 1))))");
        t.Execute("(define (bar x) (if (< x 2) 24 (foo (/ x 2))))");
        t.ExpectEq("(foo 3)", "42");
        t.ExpectEq("(foo 6)", "24");
        t.ExpectEq("(bar 7)", "42");
        t.ExpectEq("(bar 13)", "24");
    }

    {
        // CyclicLocalContextDependencies
        SchemeTest t;

        t.Execute(R"EOF(
        (define (foo x)
            (define (bar) (set! x (+ (* x 2) 2)) x)
            bar
        )
    )EOF");
        t.Execute("(define my-foo (foo 20))");
        t.Execute("(define foo 1543)");
        t.ExpectEq("(my-foo)", "42");
    }

    return 0;
}
