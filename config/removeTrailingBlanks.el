;--- 
; elist function to remove unnecessary white spaces in a buffer.
; Copy this file into your .emacs file and the feature will
; be turned on automatically
; 
; Original piece from: 
; http://lists.gnu.org/archive/html/help-gnu-emacs/2003-11/msg00290.html
;
; $Id: removeTrailingBlanks.el 1687 2005-12-18 21:40:53Z messmer $
;
(defun remove-trailing-blanks (&optional ask)
  "Removes useless blanks from a buffer.
Removes trailing spaces and tabs from every line in the current buffer,
and trailing newlines from the end of the buffer, apart from one.
If ASK is non-nil, ask for confirmation."
  (if (and (not (zerop (buffer-size)))
           (char-equal (char-after (buffer-size)) ?
)
           (save-excursion
             (save-restriction
               (save-match-data
                 (widen)
                 (goto-char 0)
                 (or (search-forward "
" nil t)
                     (search-forward "
" nil t)
                     (re-search-forward "

\`" nil t)))))
           (if ask
               (y-or-n-p "Remove trailing spaces and newlines before saving? ")
             (message "Removing trailing spaces and newlines...")
             t))
      (save-excursion
        (save-restriction
          (save-match-data
            (widen)
            (goto-char 0)
            (while (re-search-forward "[        ]+$" nil `move)
              (replace-match ""))
            (if (bolp)
                (progn
                  (skip-chars-backward "
")
                  (delete-region (1+ (point)) (point-max))))
            )))))

;; that can be hooked to several modes:
;;; Remove trailing blanks and newlines before saving a text buffer.
(add-hook 'text-mode-hook 'install-remove-trailing-blanks-ask)

(add-hook 'emacs-lisp-mode-hook 'install-remove-trailing-blanks)
(add-hook 'c-mode-hook 'install-remove-trailing-blanks)
(add-hook 'c++-mode-hook 'install-remove-trailing-blanks)
(add-hook 'octave-mode-hook 'install-remove-trailing-blanks)
(add-hook 'jde-mode-hook 'install-remove-trailing-blanks)

(defun install-remove-trailing-blanks ()
  (add-hook 'write-contents-hooks 'remove-trailing-blanks))
(defun install-remove-trailing-blanks-ask ()
  (add-hook 'write-contents-hooks '(lambda () (remove-trailing-blanks t))))

