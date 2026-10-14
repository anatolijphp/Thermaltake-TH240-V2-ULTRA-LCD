//! Just enough XML for config.xml (the .NET XmlSerializer layout the C# version wrote): elements, text, entities,
//! self-closing tags and the declaration. Attributes are skipped (only the namespace declarations appear).

#[derive(Debug, Default, Clone)]
pub struct Node {
    pub name: String,
    pub text: String,
    pub children: Vec<Node>,
}

impl Node {
    pub fn child(&self, name: &str) -> Option<&Node> {
        self.children.iter().find(|c| c.name == name)
    }
}

pub fn parse(s: &str) -> Option<Node> {
    let mut p = Parser { s: s.as_bytes(), i: 0 };
    loop {
        p.skip_ws();
        if p.starts("<?") || p.starts("<!--") || p.starts("<!") {
            p.skip_special()?;
            continue;
        }
        break;
    }
    if !p.starts("<") {
        return None;
    }
    p.element()
}

struct Parser<'a> {
    s: &'a [u8],
    i: usize,
}

impl Parser<'_> {
    fn starts(&self, t: &str) -> bool {
        self.s[self.i..].starts_with(t.as_bytes())
    }
    fn skip_ws(&mut self) {
        while self.i < self.s.len() && self.s[self.i].is_ascii_whitespace() {
            self.i += 1;
        }
    }
    fn skip_special(&mut self) -> Option<()> {
        let end = if self.starts("<?") { "?>" } else if self.starts("<!--") { "-->" } else { ">" };
        let rest = std::str::from_utf8(&self.s[self.i..]).ok()?;
        self.i += rest.find(end)? + end.len();
        Some(())
    }
    fn name(&mut self) -> String {
        let st = self.i;
        while self.i < self.s.len() && !self.s[self.i].is_ascii_whitespace() && !b"/>=".contains(&self.s[self.i]) {
            self.i += 1;
        }
        String::from_utf8_lossy(&self.s[st..self.i]).into_owned()
    }
    // at '<'
    fn element(&mut self) -> Option<Node> {
        self.i += 1;
        let mut n = Node { name: self.name(), ..Default::default() };
        // attributes: skip up to '>' or '/>' (values may contain '>' only inside quotes)
        let mut quote = 0u8;
        loop {
            let c = *self.s.get(self.i)?;
            if quote != 0 {
                if c == quote {
                    quote = 0;
                }
            } else if c == b'"' || c == b'\'' {
                quote = c;
            } else if c == b'/' && self.s.get(self.i + 1) == Some(&b'>') {
                self.i += 2;
                return Some(n);
            } else if c == b'>' {
                self.i += 1;
                break;
            }
            self.i += 1;
        }
        let mut text = Vec::new();
        loop {
            if self.i >= self.s.len() {
                return None;
            }
            if self.starts("</") {
                self.i += 2;
                self.name();
                while self.i < self.s.len() && self.s[self.i] != b'>' {
                    self.i += 1;
                }
                self.i += 1;
                break;
            }
            if self.starts("<!--") || self.starts("<?") {
                self.skip_special()?;
                continue;
            }
            if self.starts("<![CDATA[") {
                let rest = std::str::from_utf8(&self.s[self.i + 9..]).ok()?;
                let e = rest.find("]]>")?;
                text.extend_from_slice(&rest.as_bytes()[..e]);
                self.i += 9 + e + 3;
                continue;
            }
            if self.s[self.i] == b'<' {
                n.children.push(self.element()?);
                continue;
            }
            text.push(self.s[self.i]);
            self.i += 1;
        }
        n.text = unescape(&String::from_utf8_lossy(&text));
        Some(n)
    }
}

pub fn unescape(s: &str) -> String {
    if !s.contains('&') {
        return s.to_owned();
    }
    let mut o = String::with_capacity(s.len());
    let mut rest = s;
    while let Some(k) = rest.find('&') {
        o.push_str(&rest[..k]);
        rest = &rest[k..];
        let Some(e) = rest.find(';') else { break };
        let ent = &rest[1..e];
        let ch = match ent {
            "amp" => Some('&'),
            "lt" => Some('<'),
            "gt" => Some('>'),
            "quot" => Some('"'),
            "apos" => Some('\''),
            _ if ent.starts_with("#x") => u32::from_str_radix(&ent[2..], 16).ok().and_then(char::from_u32),
            _ if ent.starts_with('#') => ent[1..].parse().ok().and_then(char::from_u32),
            _ => None,
        };
        match ch {
            Some(c) => {
                o.push(c);
                rest = &rest[e + 1..];
            }
            None => {
                o.push('&');
                rest = &rest[1..];
            }
        }
    }
    o.push_str(rest);
    o
}

pub fn escape(s: &str) -> String {
    let mut o = String::with_capacity(s.len());
    for c in s.chars() {
        match c {
            '&' => o.push_str("&amp;"),
            '<' => o.push_str("&lt;"),
            '>' => o.push_str("&gt;"),
            c => o.push(c),
        }
    }
    o
}
