module PCGL
  # provides functionalities for parsing statements
  module Statement
    def self.parse(line)
      parse_rules = all_statement_classes
      parse_rules = filter_non_parseables(line, parse_rules)

      raise_too_many_rules_for(line) if parse_rules.length > 1
      if parse_rules.empty?
        parse_rules.push [Raw]
      end
      parse_rules.first.last.new(line).process
    end

    def self.all_statement_classes
      parse_rules = Base.descendants.map do |descendant|
        [descendant.priority, descendant]
      end

      parse_rules.sort_by(&:first)
    end

    def self.raise_too_many_rules_for(line)
      raise "The following line has several candidates for parsing:\n#{line}"
    end

    def self.raise_no_rules_for(line)
      raise "The following line has no candidates for parsing:\n\"#{line}\""
    end

    def self.filter_non_parseables(line, statements)
      statements.map do |rule|
        [rule.last.can_parse?(line), rule.last]
      end.keep_if(&:first)
    end
  end
end
