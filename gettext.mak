# Setup macros for gettext translations
ifdef GETTEXT_DOMAIN
  GETTEXT_SRCDIR ?= po
  GETTEXT_DESTDIR ?= locale
  GETTEXT_FROM_CODE ?= UTF-8
  GETTEXT_SRC := $(wildcard $(GETTEXT_SRCDIR)/*.po)
  GETTEXT_LANG = $(patsubst $(GETTEXT_SRCDIR)/%.po,%,$(GETTEXT_SRC))
  GETTEXT_OBJ = $(patsubst %,$(GETTEXT_DESTDIR)/%/LC_MESSAGES/$(GETTEXT_DOMAIN).mo,$(GETTEXT_LANG))
  ifdef UPDATE_POT
    GETTEXT_OBJ += $(GETTEXT_SRCDIR)/$(GETTEXT_DOMAIN).pot
  endif
endif

# Setup rules for gettext object files
.SUFFIXES: .pot .po .mo
ifdef UPDATE_POT
$(GETTEXT_SRCDIR)/$(GETTEXT_DOMAIN).pot:  $(SOURCE) $(GENERATED_SRC) $(GETTEXT_CODE)
	@echo Updating $@...
	mkdir -p $(@D)
	xgettext --debug --keyword='_' -s --foreign-user --no-location \
	    --from-code=$(GETTEXT_FROM_CODE) $(GETTEXT_FLAGS) \
	    -p $(GETTEXT_SRCDIR) -o $(GETTEXT_DOMAIN).pot \
	    $(filter-out %.fl,$^)

$(GETTEXT_SRC):  $(GETTEXT_SRCDIR)/$(GETTEXT_DOMAIN).pot
	@echo Updating $@...
	msgmerge -v -s -U --backup=off $@ $<
endif

$(GETTEXT_DESTDIR)/%/LC_MESSAGES/$(GETTEXT_DOMAIN).mo:  $(GETTEXT_SRCDIR)/%.po
	@echo Generating $@...
	mkdir -p $(@D)
	msgfmt -c -v -o $@ $<
