module PCGL
  # loads and processes a pcgl file
  class Loader
    def initialize(filename)
      @filename = filename
    end

    def process
      load(@filename)
    end

    private

    def extract_model_name(filename)
      File.basename(filename).split('.').first.to_camelcase
    end

    def load(filename)
      @root_statement = Statement::Root.new('', name: extract_model_name(filename))
      @last_statement = @root_statement
      @statement_stack = [@root_statement]
      @last_level = 0

      File.foreach(filename) do |line|
        parse_line line
      end
      apply_transformations
    end

    def parse_line(line)
      statement = Statement.parse(line).process

      indent_diff = statement.level - @last_level

      parse_indent indent_diff
      @statement_stack.last.add_child(statement)

      @last_statement = statement
      @last_level = statement.level
    end

    def parse_indent(indent_diff)
      if indent_diff == 1
        @statement_stack.push(@last_statement)
      elsif indent_diff > 1
        raise_indent_error
      else
        (-indent_diff).times { @statement_stack.pop }
      end
    end

    def raise_indent_error
      raise "line #{$INPUT_LINE_NUMBER}: ERROR INDENT DIFF IS BIGGER THAN 1"
    end

    def collect_rules
      rule_collector = Visitor::RuleCollector.new
      @root_statement.accept(rule_collector)
      rule_collector.rules
    end

    def apply_transformations
      rules = collect_rules

      rule_transformer = Visitor::RuleDefinitionTransformer.new(rules.clone)
      @root_statement.accept(rule_transformer)

      rule_transformer = Visitor::RuleConditionTransformer.new(rules.clone)
      @root_statement.accept(rule_transformer)

      set_transformer = Visitor::SetTransformer.new
      @root_statement.accept(set_transformer)

      @root_statement.debug

      visitor = Visitor::Output.new(rules.clone)
      @root_statement.accept(visitor)

      visitor
    end
  end
end
