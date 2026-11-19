#include "Core/TextEditor.h"
#include "Utils/TextProcessing.h"

#include <utility>

namespace
{
EditResult toEditResult(TextProcessing::TextStatus status)
{
    using TextProcessing::TextStatus;
    switch (status)
    {
    case TextStatus::Ok:               return EditResult::Ok;
    case TextStatus::EmptyText:        return EditResult::NothingToInsert;
    case TextStatus::InvalidCharacter: return EditResult::InvalidText;
    case TextStatus::ColumnOutOfRange: return EditResult::ColumnOutOfRange;
    }
       return EditResult::InvalidText;
}

EditResult validateReplaceText(const mystring &target, const mystring &replacement)
{
    if (target.empty())
        return EditResult::EmptyTarget;
    if (!TextProcessing::isValidLineText(target) ||
        !TextProcessing::isValidLineText(replacement))
        return EditResult::InvalidText;
    return EditResult::Ok;
}
} // namespace

EditResult TextEditor::insertText(int line, int column, const mystring &text)
{
    const int count = doc_.getLineCount();
    if (line < 1 || line > count + 1)
        return EditResult::LineOutOfRange;

    const bool pastEnd = (line == count + 1);
    const mystring none;
    const mystring &base = pastEnd ? none : doc_.getLine(line);

    mystring updated;
    const TextProcessing::TextStatus status =
        TextProcessing::insertAt(base, column, text, updated);
    if (status != TextProcessing::TextStatus::Ok)
        return toEditResult(status);

    if (pastEnd)
        doc_.insertLine(line, updated);
    else
        doc_.modifyLine(line, std::move(updated));

    files_.markModified();
    return EditResult::Ok;
}

EditResult TextEditor::insertNewLine(int line, const mystring &text)
{
    if (line < 1 || line > doc_.getLineCount() + 1)
        return EditResult::LineOutOfRange;
    if (!TextProcessing::isValidLineText(text))
        return EditResult::InvalidText;

    doc_.insertLine(line, text);
    files_.markModified();
    return EditResult::Ok;
}

EditResult TextEditor::replaceOccurrence(const mystring &target, const mystring &replacement,
                                         int occurrence, int line, ReplaceReport *report)
{
    const EditResult valid = validateReplaceText(target, replacement);
    if (valid != EditResult::Ok)
        return valid;
    if (occurrence < 1)
        return EditResult::InvalidOccurrence;
    if (line < 0 || line > doc_.getLineCount())
        return EditResult::LineOutOfRange;
    if (target == replacement)
        return EditResult::NoChange;

    int remaining = occurrence;
    int ln = 0;
    for (const Node *node = doc_.getHead(); node != nullptr; node = node->next)
    {
        ++ln;
        if (line != 0)
        {
            if (ln < line)
                continue;
            if (ln > line)
                break;
        }

        const mystring &text = node->textData;
        int pos = text.find(target, 0);
        while (pos != -1)
        {
            if (--remaining == 0)
            {
                mystring updated;
                if (!TextProcessing::replaceAt(text, pos, target.size(), replacement, updated))
                    return EditResult::NoMatchFound; // cannot happen: pos comes from find()

                doc_.modifyLine(ln, std::move(updated));
                files_.markModified();
                if (report != nullptr)
                {
                    report->count = 1;
                    report->line = ln;
                    report->column = pos + 1;
                }
                return EditResult::Ok;
            }
            pos = text.find(target, pos + 1); // +1: overlapping matches count, like search
        }
    }
    return EditResult::NoMatchFound;
}

EditResult TextEditor::replaceAll(const mystring &target, const mystring &replacement,
                                  int line, ReplaceReport *report)
{
    const EditResult valid = validateReplaceText(target, replacement);
    if (valid != EditResult::Ok)
        return valid;
    if (line < 0 || line > doc_.getLineCount())
        return EditResult::LineOutOfRange;
    if (target == replacement)
        return EditResult::NoChange;

    int total = 0;
    int ln = 0;
    for (const Node *node = doc_.getHead(); node != nullptr; node = node->next)
    {
        ++ln;
        if (line != 0)
        {
            if (ln < line)
                continue;
            if (ln > line)
                break;
        }

        mystring updated;
        const int n = TextProcessing::replaceAllInLine(node->textData, target, replacement, updated);
        if (n > 0)
        {
            doc_.modifyLine(ln, std::move(updated));
            total += n;
        }
    }

    if (total == 0)
        return EditResult::NoMatchFound;

    files_.markModified();
    if (report != nullptr)
    {
        report->count = total;
        report->line = 0;
        report->column = 0;
    }
    return EditResult::Ok;
}

const char *TextEditor::describe(EditResult r)
{
    switch (r)
    {
    case EditResult::Ok:               return "Success.";
    case EditResult::NothingToInsert:  return "Nothing to insert (the text is empty).";
    case EditResult::InvalidText:      return "Text cannot contain line breaks or NUL characters.";
    case EditResult::LineOutOfRange:   return "Line number is out of range.";
    case EditResult::ColumnOutOfRange: return "Column is out of range.";
    case EditResult::EmptyTarget:      return "The text to replace cannot be empty.";
    case EditResult::InvalidOccurrence:return "Occurrence number must be 1 or greater.";
    case EditResult::NoMatchFound:     return "No matching occurrence was found.";
    case EditResult::NoChange:         return "The replacement is identical to the original text; nothing changed.";
    }
    return "Unknown error.";
}
